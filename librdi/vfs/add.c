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

#include <driver.h>
#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>
#include <rdi/vfs.h>

/*****************************************************************************
 * vfs_add
 *
 * Adds the filesystem object <obj> to the virtual filesystem at path <path>
 * from the directory <root>; the object is given index <index> and entered
 * into the VFS index. This function is intended for internal use by drivers, 
 * esp. during init. Returns zero on success, nonzero on error.
 */

int vfs_add(struct resource *root, const char *path, struct resource *obj) {
	struct resource *dir;
	char *path1;

	if (!obj) {
		return 1;
	}

	/* find parent directory */
	path1 = path_parent(path);

	if (!root->vfs) {
		root->vfs = calloc(sizeof(struct vfs_node), 1);
		root->vfs->resource = root;
	}

	dir = vfs_find(root->vfs, path1, NULL);
	free(path1);

	if (!dir) {
		return 1;
	}

	/* push object into parent directory */
	path1 = path_name(path);
	mutex_spin(&dir->vfs->mutex);
	vfs_link(dir->vfs, path1, obj);
	mutex_free(&dir->vfs->mutex);
	free(path1);

	index_set(obj->index, obj);

	return 0;
}
