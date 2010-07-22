/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <io.h>

/****************************************************************************
 * fopen
 *
 * The fopen() function opens the file whose name is the string pointed to
 * by path and associates a stream with it.
 */

FILE *fopen(const char *path, const char *mode) {
	int fd;

	fd = find(path);

	if (fd == -1) {
		return NULL;
	}

	return fdopen(fd, mode);
}
