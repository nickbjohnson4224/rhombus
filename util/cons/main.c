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
#include <errno.h>

int main(int argc, char **argv) {
	uint64_t fobj;
	char *path;
	char type;

	if (argc < 2) {
		fprintf(stderr, "cons: missing file operand\n");
		return 1;
	}
		
	if (argv[1][0] == '-' && argc > 2) {
		type = argv[1][1];
		path = argv[2];
	}
	else {
		type = 'f';
		path = argv[1];
	}
		
	fobj = fs_find(0, path);

	if (fobj) {
		printf("cons: object %s exists\n", path);
		return 1;
	}

	switch (type) {
	case 'f': /* construct file */
		fobj = fs_cons(fs_find(0, path_parent(path)), path_name(path), FOBJ_FILE);
		break;
	case 'd': /* construct directory */
		fobj = fs_cons(fs_find(0, path_parent(path)), path_name(path), FOBJ_DIR);
		break;
	default:
		fobj = 0;
	}

	if (!fobj) {
		printf("cons: cannot construct object: \n");
		perror(NULL);
	}

	return 0;
}
