/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>

FILE *fcons(uint32_t server, uint64_t inode) {
	FILE *new = malloc(sizeof(FILE));

	if (!new) {
		return NULL;
	}

	new->server        = server;
	new->inode         = inode;
	new->position      = 0;
	new->size          = -1;
	new->buffer        = NULL;
	new->buffsize      = 0;
	new->buffpos       = 0;
	new->revbuf        = EOF;
	new->flags         = FILE_NBF | FILE_READ | FILE_WRITE;

	return new;
}
