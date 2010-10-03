/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <proc.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <natio.h>

#include "tarfs.h"

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

void tarfs_init() {
	struct tar_block *block;
	size_t i, n;

	/* allocate buffer space for header block */
	block = malloc(512);

	for (i = 0, n = 2;; n++) {

		/* read in file header block */
		fseek(parent, i, SEEK_SET);
		fread(block, 1, 512, parent);

		/* break if it's a terminating block */
		if (block->filename[0] == '\0' || block->filename[0] == ' ') {
			break;
		}

		/* add file to inode table */
		strcpy(inode[n].name, block->filename);
		inode[n].offset = i + 512;
		inode[n].size   = getvalue(block->filesize, sizeof(block->filesize));	

		/* add file to VFS */
		lfs_add(lfs_new_file(n, inode[n].size), block->filename);

		/* move to next file header */
		i += ((inode[n].size / 512) + 1) * 512;
		if (inode[n].size % 512) i += 512;
	}

	free(block);
}
