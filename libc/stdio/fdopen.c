/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * fdopen
 *
 * The fdopen() function associates a stream with the existing file 
 * descriptor fd.
 */

FILE *fdopen(int fd, const char *mode) {
	FILE *new = malloc(sizeof(FILE));

	new->filedes       = fd;
	new->position      = 0;
	new->size          = -1;
	new->buffer        = NULL;
	new->buffsize      = 0;
	new->buffpos       = 0;
	new->revbuf        = EOF;
	new->flags         = FILE_NBF | FILE_READ | FILE_WRITE;

	return new;
}
