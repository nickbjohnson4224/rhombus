/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <stdbool.h>
#include <dict.h>
#include <string.h>
#include <stdlib.h>

/****************************************************************************
 * fload
 *
 * Load a file descriptor from the dictionary. Searches for the dictionary
 * entry <name> in the namespace "file:". If it is found, a new file
 * descriptor is cloned from the dictionary version. Used to persist files
 * across process execution.
 */

FILE *fload(const char *name) {
	FILE *new;
	const FILE *old;

	/* read file from dictionary */
	old = (const FILE*) dict_readstrns("file:", name);

	/* reject invalid keys */
	if (!old) {
		return NULL;
	}

	/* allocate space for new file */
	new = malloc(sizeof(FILE));

	/* check for allocation errors */
	if (!new) {
		return NULL;
	}

	/* copy old file excluding position and buffers */
	new->server	       = old->server;
	new->inode         = old->inode;
	new->position      = old->position;
	new->size          = old->size;
	new->buffer        = NULL;
	new->buffsize      = 0;
	new->buffpos       = 0;
	new->revbuf        = old->revbuf;
	new->flags         = old->flags;

	return new;
}

int fsave(const char *name, FILE *fd) {

	/* reject null files */
	if (!fd) return 1;

	/* flush any buffers */
	fflush(fd);

	/* write file to dictionary */
	dict_writestrns("file:", name, (const uint8_t*) fd, sizeof(FILE));

	return 0;
}
