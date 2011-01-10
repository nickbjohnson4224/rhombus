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

#include <driver.h>
#include <string.h>

/*****************************************************************************
 * lfs_list
 *
 * Copy the name of the <entry>th entry in the directory <dir> into <buffer>.
 * Returns zero on success, nonzero on error.
 */

int lfs_list(struct fs_obj *dir, int entry, char *buffer, size_t size) {
	struct fs_obj *daughter;

	if (!dir) {
		return 1;
	}

	daughter = dir->daughter;

	/* select the <entry>th daughter node */
	while (daughter) {
		if (entry <= 0) {
			break;
		}
		else {
			daughter = daughter->sister1;
			entry--;
		}
	}

	if (daughter) {
		/* return name of selected daughter */
		strlcpy(buffer, daughter->name, size);
		return 0;
	}
	else {
		return 1;
	}
}
