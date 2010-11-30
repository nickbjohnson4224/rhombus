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
	char *name, list[1000];
	size_t i, l = 0;
	uint64_t dir;

	if (argc == 1) {
		dir = fs_find(0, getenv("PWD"));
	}
	else {
		dir = fs_find(0, path_simplify(argv[1]));
	}
	
	if (!dir) {
		printf("%s: no such directory\n", getenv("PWD"));
		return EXIT_FAILURE;
	}

	list[0] = '\0';

	for (i = 0;; i++) {
		name = fs_list(dir, i);

		if (name) {
			strlcat(list, name, 1000);
			l += 8 * ((strlen(name) / 8) + 1);
			if (l > 48) {
				strlcat(list, "\n", 1000);
				l = 0;
			}
			else {
				strlcat(list, "\t", 1000);
			}
			free(name);
		}
		else {
			printf("%s", list);
			if (l) printf("\n");
			return EXIT_SUCCESS;
		}
	}

	return 0;
}
