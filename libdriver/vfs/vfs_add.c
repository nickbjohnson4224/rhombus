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
#include <string.h>
#include <proc.h>

#include <driver/vfs.h>

/*****************************************************************************
 * vfs_add
 *
 * Adds the filesystem object <obj> to the virtual filesystem at path <path>
 * from the directory <root>; the object is given index <index> and entered
 * into the VFS index. This function is intended for internal use by drivers, 
 * esp. during init. Returns zero on success, nonzero on error.
 */

int vfs_add(struct vfs_obj *root, const char *path, struct vfs_obj *obj) {
	char *path1;
	uint64_t dirrp;
	struct vfs_obj *dir;
	
	if (!obj) {
		return 1;
	}

	/* find parent directory */
	path1 = path_parent(path);
	dirrp = vfs_find(root, path1, false);
	if (RP_PID(dirrp) != getpid()) {
		dir = NULL;
	}
	else {
		dir = vfs_get_index(RP_INDEX(dirrp));
	}
	free(path1);

	if (!dir) {
		return 1;
	}

	/* push object into parent directory */
	path1 = path_name(path);
	vfs_dir_push(dir, obj, path1);
	free(path1);

	return 0;
}
