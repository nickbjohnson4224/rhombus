#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inc/tar.h"

static uintptr_t getvalue(char *field, size_t size) {
	uintptr_t sum, i;

	sum = 0;

	for (i = 0; i < size && field[i]; i++) {
		sum *= 8;
		sum += field[i] - '0';
	}

	return sum;
}

size_t tar_size(uint8_t *base) {
	size_t i;
	struct tar_block *block;

	i = 0;

	while (1) {
		block = (struct tar_block*) &base[i];

		if (block->filename[0] == ' ' || block->filename[0] == '\0') {
			return i + TAR_BLOCKSIZE;
		}

		i += getvalue(block->filesize, 12) + TAR_BLOCKSIZE;

		if (i % TAR_BLOCKSIZE) {
			i = i - (i % TAR_BLOCKSIZE) + TAR_BLOCKSIZE;
		}
	}

	return 0;
}

struct tar_file *tar_parse(uint8_t *base) {
	struct tar_file *filelist;
	struct tar_block *block;
	uintptr_t i, n, filesize;
	
	filelist = malloc(sizeof(struct tar_file) * 20);
	n = 0;
	i = 0;

	while (1) {
		block = (struct tar_block*) &base[i];
		if (block->filename[0] == ' ' || block->filename[0] == '\0') {
			filelist[n].name = NULL;
			break;
		}

		filesize = getvalue(block->filesize, 12);

		filelist[n].name  = &block->filename[0];
		filelist[n].start = (void*) ((uintptr_t) block + TAR_BLOCKSIZE);
		filelist[n].size  = filesize;

		i += filesize + TAR_BLOCKSIZE;

		if (i % TAR_BLOCKSIZE) {
			i = i - (i % TAR_BLOCKSIZE) + TAR_BLOCKSIZE;
		}

		n++;
	}

	return filelist;
}

struct tar_file *tar_find(struct tar_file *archive, char *name) {
	uintptr_t i;

	for (i = 0; archive[i].name; i++) {
		if (!strcmp(name, archive[i].name)) {
			return &archive[i];
		}
	}

	return NULL;
}
