/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <string.h>

/*****************************************************************************
 * link - modify links
 *
 * SYNOPSIS
 * 
 *     link --bind <source> <dest>
 *     link -b <source> <dest>
 *     link <source> <dest>
 *
 *     Links the file pointer at <source> to a (possibly new) link at <dest>.
 *
 *     link --pointer <driver> <link>
 *     link -p <driver> <path>
 *
 *     Links the file pointer <driver> to a (possibly new) link at <path>. 
 *
 *     link --unlink <link>
 *     link -u <link>
 *
 *     Unlinks the link at <link>.
 *
 *     link --inspect <path>
 *     link -i <path>
 *
 *     Prints the file pointer at <path>. If the object at <path> is an active
 *     link, this also prints the file pointer of the hidden directory.
 */

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr, "%s: insufficient arguments\n", argv[0]);
		return 1;
	}

	return 0;
}
