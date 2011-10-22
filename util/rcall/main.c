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
#include <stdio.h>

#include <rho/natio.h>

int main(int argc, char **argv) {
	char *path, *args, *reply;
	uint64_t ctrl;
	int i;

	if (argc < 2) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
		return 1;
	}

	if (argv[1][0] == '/') {
		path = strdup(argv[1]);
	}
	else if (argv[1][0] == '@') {
		path = strdup(argv[1]);
	}
	else {
		path = strvcat("/sys/", argv[1]);
	}

	ctrl = fs_find(path);
	
	if (!ctrl) {
		fprintf(stderr, "%s: %s: control file not accessible\n", argv[0], path);
		return 1;
	}

	args = malloc(1000);
	args[0] = '\0';
	for (i = 2; i < argc; i++) {
		strlcat(args, argv[i], 1000);
		strlcat(args, " ", 1000);
	}
	if (args[0]) args[strlen(args)-1] = '\0';

	reply = rcall(ctrl, args);

	printf("%s\n", reply);
	return 0;
}
