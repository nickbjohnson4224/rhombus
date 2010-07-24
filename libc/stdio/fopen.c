/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * fopen
 *
 * The fopen() function opens the file whose name is the string pointed to
 * by path and associates a stream with it.
 */

FILE *fopen(const char *path, const char *mode) {
	FILE *new = malloc(sizeof(FILE));
	int err;

	if (!new) {
		return NULL;
	}

	err = find(path, &new->server, &new->inode);
	new->position      = 0;
	new->size          = -1;
	new->buffer        = NULL;
	new->buffsize      = 0;
	new->buffpos       = 0;
	new->revbuf        = EOF;
	new->flags         = FILE_NBF | FILE_READ | FILE_WRITE;

	if (err) {
		return NULL;
	}

	return new;
}
