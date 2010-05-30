/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vfsd.h>

void vfs_add(struct vfs *root, const char *path, uint32_t server, uint64_t inode) {
	size_t i, j;

	for (i = 0; path[i]; i++) {
		if (!root->next) {
			root->next = calloc(sizeof(void*), 128);
		}
		if (!root->next[(size_t) path[i]]) {
			root->next[(size_t) path[i]] = calloc(sizeof(struct vfs), 1);
		}
		root = root->next[(size_t) path[i]];
	}

	root->server = server;
	root->inode  = inode;
}

struct vfs *vfs_get(struct vfs *root, const char *path) {
	size_t i;

	for (i = 0; path[i]; i++) {
		if (!root || !root->next) {
			return NULL;
		}

		root = root->next[(size_t) path[i]];
	}

	return root;
}

void  vfs_lnk(struct vfs *root, const char *path, struct vfs *link) {
	if (root->link && (path[0] == '/' || path[0] == '\0')) {
		vfs_lnk(root->link, path, link);
	}

	if (path[0]) {
		if (!root->next[(size_t) path[0]]) {
			root->next[(size_t) path[0]] = calloc(sizeof(struct vfs), 1);
		}
		vfs_lnk(root->next[(size_t) path[0]], &path[1], link);
	}
	else {
		root->link = link;
	}
}
