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

#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * strparse
 *
 * Produce a list of strings from parsing the given string, and return a 
 * pointer to it. The list of strings and its contents are both in dynamically
 * allocated memory, and each must be freed; the original string is not 
 * modified. This function is also passed a list of delimiters to use when
 * parsing the string. Returns NULL on failure.
 */

char **strparse(const char *str, const char *delim) {
	char **argv;
	int argc;
	int i;

	// count potential arguments
	for (argc = 1, i = 0; str[i]; i++) {
		if (strchr(delim, str[i])) {
			argc++;
		}
	}

	// allocate argv
	argv = malloc(sizeof(char*) * (argc + 1));

	for (i = 0;; i++) {
		if (!*str) {
			break;
		}

		argv[i] = struntil(str, delim, &str);
		str += strspn(str, delim);
	}

	argv[i] = NULL;
	return argv;
}
