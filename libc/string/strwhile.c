/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <stdlib.h>

/*****************************************************************************
 * strwhile
 *
 * Returns a copy of the first part of the given string containing only
 * characters in <accept>. If <save> is non-NULL, a pointer to the first 
 * character not in <accept> is saved in <save>. Returns NULL on error.
 */

char *strwhile(const char *str, const char *accept, const char **save) {
	size_t size;
	char *ret;

	size = strspn(str, accept) + 1;
	ret = malloc(size * sizeof(char));

	memcpy(ret, str, size - 1);
	ret[size - 1] = '\0';

	if (save) {
		*save = &str[size - 1];
	}

	return ret;
}
