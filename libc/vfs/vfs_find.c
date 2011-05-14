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
#include <string.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * vfs_find
 *
 * Finds a file, starting at the filesystem object <root> in the current 
 * driver. Returns a file structure referring to the found file, which may not 
 * be in the current driver, on success, and zero on failure. If <nolink> is 
 * true, terminal links are not followed, so link objects can be found.
 */

uint64_t vfs_find(struct vfs_obj *root, const char *path_str, bool nolink) {
	struct path   *path;
	struct vfs_obj *fobj;
	struct vfs_obj *sub;
	char *name;

	path = path_cons(path_str);
	fobj = root;

	while (fobj) {
		name = path_next(path);

		if (!name) {
			if (fobj->link && !nolink) {
				return fobj->link;
			}
			else {
				return RP_CONS(getpid(), fobj->index);
			}
		}

		if ((fobj->type & FS_TYPE_DIR) == 0) {
			free(name);
			return 0;
		}
		else {
			if (fobj->link) {
				free(name);
				path_prev(path);
				return fs_find(fobj->link, path_tail(path));
			}

			if ((acl_get(fobj->acl, gettuser()) & PERM_READ) == 0) {
				return 0;
			}

			sub = fobj->daughter;

			while (sub) {
				if (!strcmp(sub->name, name)) {
					break;
				}

				sub = sub->sister1;
			}

			free(name);

			if (sub) {
				fobj = sub;
			}
			else {
				return 0;
			}
		}
	}

	return 0;
}
