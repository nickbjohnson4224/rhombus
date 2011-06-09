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

uint64_t vfs_find(struct vfs_node *root, const char *path_str, bool nolink) {
	struct resource *r;
	const char *tail;
	uint64_t rp;
	char *path;

	r = _vfs_find(root, path_str, &tail);

	if (r) {
		if (r->link) {
			if (!tail && !nolink) {
				return r->link;
			}
			else {
				path = saprintf("%r/%s", r->link, tail);
				rp = fs_find(path);
				free(path);
				return rp;
			}
		}
		else {
			return RP_CONS(getpid(), r->index);
		}
	}
	else {
		return RP_NULL;
	}
}

struct resource *_vfs_find(struct vfs_node *root, const char *path_str, const char **tail) {
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
