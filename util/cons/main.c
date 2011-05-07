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
#include <natio.h>
#include <errno.h>

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
 */ 

int main(int argc, char **argv) {
	uint64_t fobj;
	char *path;
	char type;

	if (argc < 2) {
		fprintf(stderr, "%s: missing file operand\n", argv[0]);
		return 1;
	}
		
	if (argv[1][0] == '-' && argc > 2) {
		type = argv[1][1];
		path = path_simplify(argv[2]);
	}
	else {
		type = 'f';
		path = path_simplify(argv[1]);
	}

	switch (type) {
	case 'f':
		fobj = io_cons(path, RP_TYPE_FILE);
		break;
	case 'd':
		fobj = io_cons(path, RP_TYPE_FILE);
		break;
	default:
		fobj = 0;
	}
	
	if (!fobj) {
		fprintf(stderr, "%s: cannot construct %s: ", argv[0], path);
		perror(NULL);
	}

	return 0;
}
