#include "libc.h"

#include <stdlib.h>
#include <stdint.h>

#define NUM_FILES 16

static _file_t filetable[NUM_FILES];

/* Allocate a file table entry */
_file_des_t new_file() {
	_file_des_t fd;

	fd = 0;
	while (filetable[fd].magic == 0x42242442) {
		fd++;
		if (fd > NUM_FILES) return -1;
	}

	filetable[fd].magic = 0x42242442;

	return fd;
}

void del_file(_file_des_t fd) {
	filetable[fd].magic = 0;
}

_file_t *get_file(_file_des_t fd) {
	if (fd > NUM_FILES) return NULL;

	return &filetable[fd];
}
