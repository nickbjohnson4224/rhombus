/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <stdio.h>
#include <natio.h>
#include <errno.h>

static char buffer[2048];

int main(int argc, char **argv) {
	FILE *dest;
	size_t size;

	if (argc < 2) {
		fprintf(stderr, "write: missing destination\n");
		return EXIT_FAILURE;
	}

	dest = fopen(argv[2], "w");
	if (!dest) {
		fprintf(stderr, "write: cannot open %s: ", argv[1]);
		perror(NULL);
		return EXIT_FAILURE;
	}

	while (1) {
		size = fread(buffer, sizeof(char), 2048, stdin);
		if (!size) {
			break;
		}
		size = fwrite(buffer, sizeof(char), size, stdout);
		if (!size) {
			fprintf(stderr, "error writing to %s ", argv[1]);
			perror(NULL);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
