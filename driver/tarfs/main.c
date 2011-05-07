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
#include <vfs.h>

#include "tarfs.h"

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

size_t tarfs_read(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	struct vfs_obj *file;

	file = vfs_get(index);

	if (!file->data) {
		return 0;
	}

	if (offset >= file->size) {
		return 0;
	}

	if (offset + size >= file->size) {
		size = file->size - offset;
	}

	fseek(parent, (size_t) file->data + offset, SEEK_SET);
	fread(buffer, 1, size, parent);

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
	struct vfs_obj *file, *root;
	size_t i, n;

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

	/* create root directory */
	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_DIR;
	root->acl = acl_set_default(root->acl, PERM_READ);
	vfs_set(0, root);

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
			file        = calloc(sizeof(struct vfs_obj), 1);
			file->type  = RP_TYPE_DIR;
			file->index = n;
			file->acl   = acl_set_default(file->acl, PERM_READ);
			vfs_add(root, block->filename, file);

		}
		else {

			/* add file to VFS */
			file        = calloc(sizeof(struct vfs_obj), 1);
			file->type  = RP_TYPE_FILE;
			file->index = n;
			file->data  = (uint8_t*) (i + 512);
			file->size  = getvalue(block->filesize, 12);
			file->acl   = acl_set_default(file->acl, PERM_READ);
			vfs_add(root, block->filename, file);

		}

		/* move to next file header */
		i += ((file->size / 512) + 1) * 512;
		if (file->size % 512) i += 512;
	}

	free(block);

	/* set up interface */
	di_wrap_read(tarfs_read);
	vfs_init();

	/* daemonize */
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return EXIT_SUCCESS;
}
