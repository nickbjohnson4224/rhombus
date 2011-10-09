/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

#include <rdi/vfs.h>
#include <rdi/io.h>

static FILE *parent = NULL;

/****************************************************************************
 * tar_block
 *
 * Tape archive header structure.
 */

struct tar_block {
	char filename[100];
	char mode[8];
	char owner[8];
	char group[8];
	char filesize[12];
	char timestamp[12];
	char checksum[8];
	char link[1];
	char linkname[100];
};

/****************************************************************************
 * getvalue
 *
 * Returns the value of the given tar header field. Effectively the same as
 * a bounded atoo().
 */

static uintptr_t getvalue(char *field, size_t size) {
	uintptr_t sum, i;

	sum = 0;

	for (i = 0; i < size && field[i]; i++) {
		sum *= 8;
		sum += field[i] - '0';
	}

	return sum;
}

char *tarfs_cons(struct robject *self, rp_t source, int argc, char **argv) {
	struct robject *new_r = NULL;
	char *type;

	if (argc == 2) {
		type = argv[1];

		if (!strcmp(type, "link")) {
			new_r = rdi_link_cons(robject_new_index(), PERM_READ | PERM_WRITE, NULL);
		}
		else {
			return strdup("! type");
		}

		if (new_r) {
			return rtoa(RP_CONS(getpid(), new_r->index));
		}
	}

	return strdup("! arg");
}

size_t tarfs_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	off_t *file_size;
	off_t *file_poff;

	mutex_spin(&self->driver_mutex);

	file_size = robject_data(self, "size");
	file_poff = robject_data(self, "parent-offset");

	if (!file_size || !file_poff) return 0;

	if (offset >= *file_size) {
		return 0;
	}

	if (offset + size > *file_size) {
		size = *file_size - offset;
	}

	fseek(parent, *file_poff + offset, SEEK_SET);
	fread(buffer, 1, size, parent);

	mutex_free(&self->driver_mutex);

	return size;
}

/****************************************************************************
 * tarfs - tape archive filesystem driver
 *
 * Usage: tarfs [parent]
 *
 * Constructs a filesystem using the tarfile parent as a parent driver.
 */

int main(int argc, char **argv) {
	struct tar_block *block;
	struct robject *root;
	struct robject *file;
	size_t i, n;
	off_t *poff, *size;

	/* reject if no parent is speicified */
	if (argc < 2) {
		fprintf(stderr, "%s: no parent driver specified\n", argv[0]);
		abort();
	}
	else {

		/* get parent driver stream */
		parent = fopen(argv[1], "r");

		if (!parent) {
			/* parent does not exist - fail */
			fprintf(stderr, "%s: no parent driver %s\n", argv[0], argv[1]);
			abort();
		}
	}

	rdi_init();

	/* create root directory */
	root = rdi_dir_cons(0, PERM_READ);
	robject_set(0, root);
	robject_root = root;

	/* allocate buffer space for header block */
	block = malloc(512);

	for (i = 0, n = 1;; n++) {

		/* read in file header block */
		fseek(parent, i, SEEK_SET);
		fread(block, 1, 512, parent);

		/* break if it's a terminating block */
		if (block->filename[0] == '\0' || block->filename[0] == ' ') {
			break;
		}

		if (block->filename[strlen(block->filename) - 1] == '/') {

			/* add directory to VFS */
			block->filename[strlen(block->filename) - 1] = 0;
			file = rdi_dir_cons(robject_new_index(), PERM_READ);
			rdi_vfs_add(root, block->filename, file);
	
			/* move to next file header */
			i += 512;
		}
		else {

			/* add file to VFS */
			file = rdi_file_cons(robject_new_index(), PERM_READ);
			rdi_vfs_add(root, block->filename, file);

			poff = malloc(sizeof(off_t));
			size = malloc(sizeof(off_t));
			*poff = i + 512;
			*size = getvalue(block->filesize, 12);
			robject_set_data(file, "size", size);
			robject_set_data(file, "parent-offset", poff);
			
			/* move to next file header */
			i += ((*size / 512) + 1) * 512;
			if (*size % 512) i += 512;
		}
	}

	free(block);

	/* set up interface */
	robject_set_call(rdi_class_core, "cons", tarfs_cons);
	rdi_global_read_hook = tarfs_read;

	/* daemonize */
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return EXIT_SUCCESS;
}
