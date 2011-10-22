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
#include <stdio.h>
#include <errno.h>

#include <rho/natio.h>

/*****************************************************************************
 * cons - create filesystem objects
 *
 * SYNOPSIS
 *
 *     cons --file <filename>
 *     cons -f <filename>
 *     cons <filename>
 *
 *     Creates an empty file at the path <filename>.
 *
 *     cons --dir <path>
 *     cons -d <path>
 *     
 *     Creates an empty directory at the path <path>.
 *
 *     cons --link <path>
 *     cons -l <path>
 *
 *     Creates a blank symbolic link at path <path>.
 */ 

int main(int argc, char **argv) {
	uint64_t fobj;
	char *path;
	const char *type;

	if (argc < 2) {
		fprintf(stderr, "%s: missing file operand\n", argv[0]);
		return 1;
	}
	
	if (argc > 2) {

		if (argv[1][0] == '-') {
			switch (argv[1][1]) {
			case 'd': type = "dir"; break;
			case 'f': type = "file"; break;
			case 'l': type = "link"; break;
			default: 
				fprintf(stderr, "%s: unknown type %c\n", argv[0], argv[1][1]);
				return 1; break;
			}
		}
		else {
			type = argv[1];
		}

		path = path_simplify(argv[2]);
	}
	else {
		type = "file";
		path = path_simplify(argv[1]);
	}

	fobj = fs_cons(path, type);
	
	if (!fobj) {
		fprintf(stderr, "%s: cannot construct %s: ", argv[0], path);
		perror(NULL);
	}

	return 0;
}
