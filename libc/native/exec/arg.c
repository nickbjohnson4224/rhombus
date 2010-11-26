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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pack.h>

/****************************************************************************
 * argv_pack
 *
 * Copies an argument list to the dictionary in the namespace "arg:", for 
 * later retrieval.
 */

void argv_pack(int argc, const char **argv) {
	size_t i;

	__pack_add(PACK_KEY_ARG | 0, &argc, sizeof(int));

	for (i = 0; i < (size_t) argc; i++) {
		__pack_add(PACK_KEY_ARG | (i + 1), argv[i], strlen(argv[i]) + 1);
	}
}

/****************************************************************************
 * argc_unpack
 *
 * Returns the value of argc packed in the dictionary.
 */

int argc_unpack(void) {
	size_t length;
	const int *value;

	value = __pack_load(PACK_KEY_ARG, &length);

	if (!value || length != sizeof(int)) {
		return 0;
	}
	else {
		return (*value);
	}
}

/****************************************************************************
 * argv_unpack
 *
 * Returns the value of argv packed in the dictionary.
 */

char **argv_unpack(void) {
	int argc;
	size_t i, length;
	const char *value;
	char **argv;

	argc = argc_unpack();
	argv = malloc(sizeof(char*) * (argc + 1));

	for (i = 0; i < (size_t) argc; i++) {
		value = __pack_load(PACK_KEY_ARG | (i + 1), &length);

		if (!value) {
			argv[i] = NULL;
		}
		else {
			argv[i] = malloc(strlen(value) + 1);
			strcpy(argv[i], value);
		}
	}

	argv[i] = NULL;

	return argv;
}
