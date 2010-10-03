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

#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <string.h>

int main(int argc, char **argv) {
	char *list;
	size_t n, i, j;

	if (argc == 1) {
		list = vfs_get_list(NULL, getenv("PWD"));
		
		if (!list) {
			printf("%s: no such directory\n", getenv("PWD"));
			return EXIT_FAILURE;
		}

		for (i = 0, j = 1; list[i]; i++) {
			if (list[i] == ':') {
				list[i] = (j % 6) ? '\t' : '\n';
				j++;
			}
		}

		printf("%s\n", list);
		free(list);
	}

	else for (n = 1; n < (size_t) argc; n++) {

		if (argc > 2) {
			printf("%s:\n", argv[n]);
		}

		list = vfs_get_list(NULL, argv[n]);

		if (!list) {
			printf("%s: no such directory\n", argv[n]);
			continue;
		}

		for (i = 0, j = 1; list[i]; i++) {
			if (list[i] == ':') {
				list[i] = (j % 6) ? '\t' : '\n';
				j++;
			}
		}

		printf("%s\n", list);
		free(list);
	}

	return 0;
}
