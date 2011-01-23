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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pack.h>

/****************************************************************************
 * packarg
 *
 * Convert the argument list <argv> into a contiguous page-aligned pack.
 * This pack is backspace terminated.
 */

char *packarg(const char **argv) {
	size_t size, i;
	char *pack, *top;
	
	size = 1;
	for (i = 0; argv[i]; i++) {
		size += strlen(argv[i]) + 1;
	}

	pack = aalloc(size, PAGESZ);

	top = pack;
	for (i = 0; argv[i]; i++) {
		strcpy(top, argv[i]);
		top += strlen(top) + 1;
	}
	top[0] = '\b';

	return pack;
}

/****************************************************************************
 * loadarg
 *
 * Convert the pack <pack> into an argument list.
 */

char **loadarg(char *pack) {
	size_t i;
	char *top;
	char **argv;

	argv = NULL;
	top = pack;
	for (i = 0;; i++) {
		argv = realloc(argv, (i + 1) * sizeof(char*));
		
		if (top[0] == '\b') {
			argv[i] = NULL;
			break;
		}
		else {
			argv[i] = strdup(top);
		}

		top += strlen(top) + 1;
	}

	return argv;
}
