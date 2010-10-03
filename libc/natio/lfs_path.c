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

#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <mutex.h>

static struct lfs_node _lfs_root = {
	0,
	0,
	NULL,
	NULL,
	VFS_DIR,
	"<root>",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0xFFFF,
	0xFFFF,
};

static bool _m_lfs_root;

static uint32_t _add_rec(struct lfs_node *root, struct path *path, struct lfs_node *node);
static struct lfs_node *_get_rec(struct lfs_node *root, struct path *path);
static uint32_t _add_child(struct lfs_node *parent, struct lfs_node *child, const char *name);

/****************************************************************************
 * _add_child
 *
 * Adds a child LFS node to an LFS node. Returns zero on success, nonzero on
 * failure.
 */

static uint32_t _add_child(struct lfs_node *parent, struct lfs_node *child, const char *name) {
	struct lfs_node *sister;

	child->name = name;

	sister = parent->daughter;
	while (sister) {
		if (!strcmp(sister->name, name)) {
			return VFS_ERR | VFS_FILE;
		}

		if (sister->sister1) {
			sister = sister->sister1;
		}
		else {
			break;
		}
	}

	if (sister) {
		sister->sister1 = child;
		child->sister0  = sister;
		child->sister1  = NULL;
		child->mother   = parent;
	}
	else {
		parent->daughter = child;
		child->mother    = parent;
		child->sister0   = NULL;
		child->sister1   = NULL;
	}

	return 0;
}

/****************************************************************************
 * _add_rec
 *
 * Adds a node <node> to the LFS tree recursively, using the path <path> and 
 * the current tree position <root>. Returns zero on success, nonzero on 
 * error.
 */

static uint32_t _add_rec(struct lfs_node *root, struct path *path, struct lfs_node *node) {
	char *name;
	struct lfs_node *child;
	uint32_t err;

	name = path_next(path);

	if (name == NULL) {
		return VFS_ERR | VFS_PATH;
	}

	if (path_peek(path) == NULL) {
		return _add_child(root, node, name);
	}

	child = root->daughter;
	while (child) {
		if (!strcmp(child->name, name)) {
			switch (child->type) {
			case VFS_FILE:
				free(name);
				return VFS_ERR | VFS_TYPE;
			case VFS_DIR:
				err = _add_rec(child, path, node);
				free(name);
				return err;
			case VFS_LINK:
				free(name);
				return VFS_ERR | VFS_LINK;
			}
		}
		child = child->sister1;
	}

	return VFS_ERR | VFS_PATH;
}

/****************************************************************************
 * lfs_add_path
 *
 * Adds the node <node> to the LFS tree at <path> starting at <root>. Returns 
 * zero on success, nonzero on error. This function is thread-safe.
 */

uint32_t lfs_add_path(struct lfs_node *root, const char *path, struct lfs_node *node) {
	uint32_t err;
	struct path *path_struct;

	if (!root) {
		root = &_lfs_root;
	}

	path_struct = path_cons(path);

	mutex_spin(&_m_lfs_root);
	err = _add_rec(root, path_struct, node);
	mutex_free(&_m_lfs_root);

	free(path_struct);

	return err;
}

/****************************************************************************
 * _get_rec
 *
 * Locates the node in the LFS tree with path <path> recursively, using
 * <root> as the current tree position. Returns a pointer to the found node
 * on success, NULL on failure.
 */

static struct lfs_node *_get_rec(struct lfs_node *root, struct path *path) {
	char *name;
	struct lfs_node *child;

	name = path_next(path);

	if (name == NULL) {
		return root;
	}
	else if (root->type == VFS_DIR) {
		child = root->daughter;
		while (child) {
			if (!strcmp(child->name, name)) {
				free(name);
				return _get_rec(child, path);
			}
			child = child->sister1;
		}
		free(name);
		return NULL;
	}
	else {
		free(name);
		return NULL;
	}
}

/****************************************************************************
 * lfs_get_path
 *
 * Locates the node in the LFS tree with path <path> starting at <root>. 
 * Returns a pointer to the found node on success, NULL on failure. This 
 * function is thread-safe.
 */

struct lfs_node *lfs_get_path(struct lfs_node *root, const char *path) {
	struct path *path_struct;
	struct lfs_node *ret;

	if (!root) {
		root = &_lfs_root;
	}

	path_struct = path_cons(path);

	mutex_spin(&_m_lfs_root);
	ret = _get_rec(root, path_struct);
	mutex_free(&_m_lfs_root);

	free(path_struct);

	return ret;
}

/****************************************************************************
 * lfs_del_path
 *
 * Removes the node with path <path> from the LFS tree starting at <root>. 
 * Returns a pointer to the removed node on success, NULL on failure. This 
 * function is thread-safe.
 */

struct lfs_node *lfs_del_path(struct lfs_node *root, const char *path) {
	struct lfs_node *node;

	node = lfs_get_path(root, path);

	if (!node) {
		return NULL;
	}

	if (node->sister0) {
		node->sister0->sister1 = node->sister1;
	}
	else {
		node->mother->daughter = NULL;
	}

	if (node->sister1) {
		node->sister1->sister0 = node->sister0;
	}

	return node;
}

/****************************************************************************
 * _get_link_rec
 *
 */

static struct lfs_node *_get_link_rec(struct lfs_node *root, struct path *path, const char **tail) {
	char *name;
	struct lfs_node *child;

	name = path_next(path);

	if (name == NULL) {
		if (root->type == VFS_LINK) {
			*tail = path_tail(path);
			return root;
		}
		else {
			return NULL;
		}
	}

	if (root->type == VFS_DIR) {
		child = root->daughter;
		while (child) {
			if (!strcmp(child->name, name)) {
				free(name);
				return _get_link_rec(child, path, tail);
			}
			child = child->sister1;
		}
		free(name);
		return NULL;
	}
	else if (root->type == VFS_LINK) {
		free(name);
		path_prev(path);
		*tail = path_tail(path);
		return root;
	}
	else {
		free(name);
		return NULL;
	}
}

/****************************************************************************
 * lfs_get_link
 *
 * Locates the first link node in the LFS tree along path <path> starting
 * at <root>. Returns a pointer to the found node and sets <tail> to the
 * remaining path string on success, and returns NULL on failure. This
 * function is thread-safe.
 */

struct lfs_node *lfs_get_link(struct lfs_node *root, const char *path, const char **tail) {
	struct lfs_node *node;
	struct path *path_struct;

	if (!root) {
		root = &_lfs_root;
	}

	path_struct = path_cons(path);

	mutex_spin(&_m_lfs_root);
	node = _get_link_rec(root, path_struct, tail);
	mutex_free(&_m_lfs_root);

	free(path_struct);

	return node;
}
