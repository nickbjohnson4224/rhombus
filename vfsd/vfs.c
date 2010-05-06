/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <vfsd.h>

void vfs_add(struct vfs *root, const char *path, uint32_t server, uint64_t inode) {
	if (root->link && (path[0] == '/' || path[0] == '\0')) {
		vfs_add(root->link, path, server, inode);
	}

	if (path[0]) {
		if (!root->next[path[0]]) {
			root->next[path[0]] = calloc(sizeof(struct vfs), 1);
			root->refc++;
		}
		vfs_add(root->next[path[0]], &path[1], server, inode);
	}
	else {
		root->server = server;
		root->inode  = inode;
	}
}

struct vfs *vfs_get(struct vfs *root, const char *path) {
	if (root->link && (path[0] == '/' || path[0] == '\0')) {
		return vfs_get(root->link, path);
	}

	if (path[0]) {
		if (!root->next[path[0]]) return NULL;
		else return vfs_get(root->next[path[0]], &path[1]);
	}
	else {
		return root;
	}
}

void  vfs_lnk(struct vfs *root, const char *path, struct vfs *link) {
	if (root->link && (path[0] == '/' || path[0] == '\0')) {
		vfs_lnk(root->link, path, link);
	}

	if (path[0]) {
		if (!root->next[path[0]]) {
			root->next[path[0]] = calloc(sizeof(struct vfs), 1);
			root->refc++;
		}
		vfs_lnk(root->next[path[0]], &path[1], link);
	}
	else {
		root->link = link;
	}
}

bool vfs_rem(struct vfs *root, const char *path) {
	int f;

	if (root->link && (path[0] == '/' || path[0] == '\0')) {
		f = vfs_rem(root->link, path);
		if (f) root->link = NULL;
		return false;
	}

	if (path[0]) {
		if (!root->next[path[0]]) return 0;
		else {
			f = vfs_rem(root->next[path[0]], &path[1]);
			if (f) {
				free(root->next[path[0]]);
				root->next[path[0]] = NULL;
				root->refc--;
			}
			if (!(root->refc || root->server || root->link)) {
				return true;
			}
		}
	}
	else {
		return true;
	}
	
	return false;
}
