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

#include <dict.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/****************************************************************************
 * argv_pack
 *
 * Copies an argument list to the dictionary in the namespace "arg:", for 
 * later retrieval.
 */

void argv_pack(int argc, const char **argv) {
	int i, j, k;
	char buffer[12];

	dict_writestr("arg:argc", (const uint8_t*) &argc, sizeof(int));

	for (i = 0; i < argc; i++) {

		/* XXX - replace with sprintf when possible */
		for (k = 0, j = i; j > 0; j /= 10) {
			buffer[k++] = '0' + (j % 10);
		}
		buffer[k] = '\0';

		dict_writestrns("arg:", buffer, 
			(const uint8_t*) argv[i], strlen(argv[i]) + 1);
	}
}

/****************************************************************************
 * argc_unpack
 *
 * Returns the value of argc packed in the dictionary.
 */

int argc_unpack(void) {
	const int *packed;

	packed = (const int*) dict_readstr("arg:argc");

	if (!packed) {
		return 0;
	}
	else {
		return *packed;
	}
}

/****************************************************************************
 * argv_unpack
 *
 * Returns the value of argv packed in the dictionary.
 */

char **argv_unpack(void) {
	int argc, i, j, k;
	const char *packed;
	char **argv, buffer[12];

	argc = argc_unpack();
	argv = malloc(sizeof(char*) * (argc + 1));

	for (i = 0; i < argc; i++) {
		
		for (k = 0, j = i; j > 0; j /= 10) {
			buffer[k++] = '0' + (j % 10);
		}
		buffer[k] = '\0';

		packed = (const char*) dict_readstrns("arg:", buffer);

		if (!packed) {
			argv[i] = NULL;
		}
		else {
			argv[i] = malloc(strlen(packed) + 1);
			strcpy(argv[i], packed);
		}
	}

	argv[i] = NULL;

	return argv;
}
