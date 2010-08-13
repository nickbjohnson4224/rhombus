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

#include <stdio.h>
#include <stdbool.h>
#include <dict.h>
#include <string.h>
#include <stdlib.h>

/****************************************************************************
 * __fload
 *
 * Load a file descriptor from the dictionary. Searches for the dictionary
 * entry <name> in the namespace "file:". If it is found, a new file
 * descriptor is cloned from the dictionary version. Used to persist files
 * across process execution.
 */

FILE *__fload(const char *name) {
	FILE *new;
	size_t length;

	/* allocate space for new file */
	new = malloc(sizeof(FILE));

	/* check for allocation errors */
	if (!new) {
		return NULL;
	}

	/* read file from dictionary */
	length = inflate(new, sizeof(FILE), dreadns("file:", name));
	
	/* reject invalid keys */
	if (length != sizeof(FILE)) {
		free(new);
		return NULL;
	}

	/* reset position and buffers */
	new->buffer        = NULL;
	new->buffsize      = 0;
	new->buffpos       = 0;

	return new;
}
