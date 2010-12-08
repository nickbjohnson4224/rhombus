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
#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * lfs_add
 *
 * Adds the filesystem object <obj> to the local filesystem at path <path>.
 * This function is intended for internal use by drivers, esp. during init.
 */

void lfs_add(struct fs_obj *obj, const char *path) {
	char *path1;
	uint64_t dirfd;
	struct fs_obj *dir;
	
	if (!obj) {
		return;
	}

	/* find parent directory */
	path1 = path_parent(path);
	dirfd = lfs_find(0, path1, false);
	dir = lfs_lookup(dirfd & 0xFFFFFFFF);
	free(path1);

	if (!dir) {
		return;
	}

	/* push object into parent directory */
	path1 = path_name(path);
	lfs_push(dir, obj, path1);
	free(path1);
}
