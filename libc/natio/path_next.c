/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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
#include <natio.h>

/****************************************************************************
 * path_next
 *
 * Returns a string containing the next token in the path structure, and
 * advances the position of the path structure. This string is allocated from 
 * the heap and should be freed. Returns NULL if there are no more tokens
 * left.
 */

char *path_next(struct path *path) {
	char *token, *end;

	/* check for problems */
	if (!path || !path->pos) {
		return NULL;
	}

	/* skip leading separators */
	while (*(path->pos) == PATH_SEP) {
		path->pos++;
	}

	/* check for end of string */
	if (!*(path->pos)) {
		return NULL;
	}
	
	/* copy token */
	token = strdup(path->pos);
	end = strchr(token, PATH_SEP);
	if (end) {
		*end = '\0';
	}

	/* advance path structure */
	path->pos = strchr(path->pos, PATH_SEP);
	if (!path->pos) {
		path->pos = &path->str[strlen(path->str)];
	}

	return token;
}
