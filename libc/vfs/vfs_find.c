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
 * Finds a resource, starting at the filesystem node <root>. Returns the found
 * resource structure on success, and NULL on failure. If a symbolic link is 
 * encountered before the proper resource, the link's resource is returned 
 * instead, and the remaining path string is stored in *<tail> if <tail> is
 * non-NULL.  
 */

struct resource *vfs_find(struct vfs_node *root, const char *path_str, const char **tail) {
	struct path *path;
	struct vfs_node *child;
	char *name;

	path = path_cons(path_str);

	while (root) {
		name = path_next(path);

		if (!name) {
			if (tail) *tail = NULL;
			free(path);
			return root->resource;
		}

		if (FS_IS_LINK(root->resource->type) && root->resource->symlink) {
			path_prev(path);
			if (tail) *tail = path_tail(path);
			free(path);
			return root->resource;
		}

		if (root->resource->link) {
			path_prev(path);
			if (tail) *tail = path_tail(path);
			free(path);
			return root->resource;
		}

		child = root->daughter;
		while (child) {
			if (!strcmp(child->name, name)) {
				break;
			}

			child = child->sister1;
		}

		free(name);
		root = child;
	}

	return NULL;
}
