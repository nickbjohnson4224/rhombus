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
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

/****************************************************************************
 * strvcat
 *
 * Concatenates a list of strings and returns the result as a buffer in
 * the global heap. Returns NULL on error.
 */

char *strvcat(const char *s1, ...) {
	va_list ap;
	const char *argv[16];
	char *fullstring;
	size_t i, length, pos;

	va_start(ap, s1);

	argv[0] = s1;
	
	for (i = 1; i < 16; i++) {
		argv[i] = va_arg(ap, const char*);
		if (!argv[i]) {
			break;
		}
	}

	for (length = 0, i = 0; i < 16 && argv[i]; i++) {
		length += strlen(argv[i]);
	}

	fullstring = malloc(length + 1);

	if (!fullstring) {
		errno = ENOMEM;
		return NULL;
	}

	fullstring[0] = '\0';

	for (i = 0, pos = 0; i < 16 && argv[i]; i++) {
		strcat(&fullstring[pos], argv[i]);
		pos += strlen(argv[i]);
	}

	return fullstring;
}
