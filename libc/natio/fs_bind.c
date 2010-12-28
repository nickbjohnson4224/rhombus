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

#include <stdint.h>
#include <stdlib.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_bind
 *
 * Creates and sets the link at <path> to point to <fobj>. Returns zero on
 * success, nonzero on error.
 */

int fs_bind(uint64_t fobj, const char *path) {
	uint64_t dir, link;
	char *str;

	if (fs_find(0, path)) {
		link = fs_find(0, path);
	}
	else {
		str = path_parent(path);
		if (!str) return 1;
		dir = fs_find(0, str);
		if (!dir) return 1;
		free(str);

		str = path_name(path);
		if (!str) return 1;
		link = fs_cons(dir, str, FOBJ_DIR);
		if (!link) return 1;
		free(str);
	}

	return fs_link(link, fobj);
}
