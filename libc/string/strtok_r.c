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
#include <stdint.h>
#include <stdbool.h>

char *strtok_r(char *str, const char *delim, char **saveptr) {
	size_t i;
	size_t j;
	bool delimp;

	if (str) {
		*saveptr = str;
	}
	else {
		if (!*saveptr) {
			return NULL;
		}
		else {
			str = *saveptr;
		}
	}

	/* skip leading delimiter characters */
	for (i = 0; str[i]; i++) {
		delimp = false;

		for (j = 0; delim[j]; j++) {
			if (str[i] == delim[j]) {
				delimp = true;
				break;
			}
		}

		if (!delimp) {
			break;
		}
	}

	/* skip non-delimiters until one is found */
	for (; str[i]; i++) {
		for (j = 0; delim[j]; j++) {
			if (str[i] == delim[j]) {

				/* terminate token and return */
				str[i] = '\0';
				*saveptr = &str[i + 1];
				return str;
			}
		}
	}

	*saveptr = NULL;
	return str;
}
