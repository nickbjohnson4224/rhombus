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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <errno.h>

int main(int argc, char **argv) {
	char *list, *path;
	char **listv;
	uint64_t dir;
	int i;

	if (argc == 1) {
		path = (char*) getenv("PWD");
	}
	else {
		path = argv[1];
	}

	dir = fs_open(path, STAT_READER);

	if (!dir) {
		fprintf(stderr, "%s: ", path_simplify(path));
		perror(NULL);
		abort();
	}

	errno = 0;
	list = rp_list(dir);
	if (!list) {
		if (errno) {
			fprintf(stderr, "%s: ", path);
			perror(NULL);
			abort();
		}
		else {
			list = strdup("");
		}
	}

	listv = strparse(list, "\t");

	for (i = 0; listv[i]; i++) {
		printf("%s\t", listv[i]);
		if (i % 8 == 7 && listv[i+1]) printf("\n");
	}
	printf("\n");

	return EXIT_SUCCESS;
}
