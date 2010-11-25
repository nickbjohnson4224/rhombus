/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <string.h>
#include <stdlib.h>
#include <driver.h>
#include <mutex.h>

#include "tarfs.h"

static FILE *parent = NULL;
static bool m_parent = false;

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

/****************************************************************************
 * tarfs_init
 *
 * Initializes the tarfs driver with the stream "parent" as its parent
 * driver. Adds all files to the VFS and to the local inode table.
 */

void tarfs_init(int argc, char **argv) {
	struct tar_block *block;
	struct fs_obj *file;
	size_t i, n;

	/* reject if no parent is speicified */
	if (argc < 2) {
		fprintf(stderr, "%s: no parent driver specified", argv[0]);
		abort();
	}

	/* get parent driver stream */
	parent = fopen(argv[1], "r");

	if (!parent) {
		/* parent does not exist - fail */
		fprintf(stderr, "%s: no parent driver %s\n", argv[0], argv[1]);
		abort();
	}

	/* create root directory */
	file = calloc(sizeof(struct fs_obj), 1);
	file->type = FOBJ_DIR;
	file->inode = 0;
	lfs_root(file);

	/* allocate buffer space for header block */
	block = malloc(512);

	for (i = 0, n = 1;; n++) {

		/* read in file header block */
		fseek(parent, i, SEEK_SET);
		if (!fread(block, 1, 512, parent)) printf("!!!\n");

		/* break if it's a terminating block */
		if (block->filename[0] == '\0' || block->filename[0] == ' ') {
			break;
		}

		/* add file to VFS */
		file = calloc(sizeof(struct fs_obj), 1);
		file->type  = FOBJ_FILE;
		file->inode = n;
		file->data  = (uint8_t*) (i + 512);
		file->size  = getvalue(block->filesize, 12);
		lfs_add(file, block->filename);

		/* move to next file header */
		i += ((file->size / 512) + 1) * 512;
		if (file->size % 512) i += 512;
	}

	free(block);
}

struct fs_obj *tarfs_cons(int type) {
	return NULL;
}

int tarfs_push(struct fs_obj *obj) {
	return 0;
}

int tarfs_pull(struct fs_obj *obj) {
	return 0;
}

int tarfs_free(struct fs_obj *obj) {
	return 1;
}

uint64_t tarfs_size(struct fs_obj *file) {	
	return file->size;
}

size_t tarfs_read(struct fs_obj *file, void *buffer, size_t size, uint64_t offset) {
	
	if (!file->data) {
		return 0;
	}

	if (offset >= file->size) {
		return 0;
	}

	if (offset + size >= file->size) {
		size = file->size - offset;
	}

	mutex_spin(&m_parent);

	fseek(parent, (size_t) file->data + offset, SEEK_SET);
	fread(buffer, 1, size, parent);

	mutex_free(&m_parent);
	
	return size;
}

size_t tarfs_write(struct fs_obj *file, void *buffer, size_t size, uint64_t offset) {
	return 0;
}

int tarfs_reset(struct fs_obj *file) {
	return 1;
}

int tarfs_sync(struct fs_obj *file) {
	return 1;
}

struct driver tarfs_driver = {
	tarfs_init, 

	tarfs_cons,
	tarfs_push,
	tarfs_pull,
	tarfs_free,

	tarfs_size,
	tarfs_read,
	tarfs_write,
	tarfs_reset,
	tarfs_sync,
};
