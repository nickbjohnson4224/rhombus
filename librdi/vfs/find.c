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

#include <rdi/vfs.h>

#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>

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

/*****************************************************************************
 * __rdi_find_handler
 */

char *__rdi_find_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *root;
	struct resource *file;
	const char *path;
	const char *tail;
	bool link;

	if (argc <= 1) return NULL;

	// check for follow link flag
	if (argc == 2) {
		link = false;
		path = argv[1];
	}
	else if (!strcmp(argv[1], "-L")) {
		link = true;
		path = argv[2];
	}
	else {
		return NULL;
	}

	// find root node
	root = index_get(index);
	if (!root) return strdup("! nfound");

	// find resource
	if (!root->vfs && (path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))) {
		file = root;
		tail = NULL;
	}
	else {
		file = vfs_find(root->vfs, path, &tail);
	}

	if (!file) return strdup("! nfound");

	if (file->symlink && !(!tail && link)) {
		/* return redirect to symlink */
		return saprintf(">> %s/%s", file->symlink, (tail) ? tail : "");
	}
	else {
		/* return resource pointer */
		return rtoa(RP_CONS(getpid(), file->index));
	}
}
