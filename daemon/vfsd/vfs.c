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
		if (root->link && (path[i] == '/' || path[i] == '\0')) {
			root = root->link;
		}
		else {
			if (!root->next) {
				root->next = calloc(sizeof(void*), 128);
			}
			if (!root->next[(size_t) path[i]]) {
				root->next[(size_t) path[i]] = calloc(sizeof(struct vfs), 1);
				root->next[(size_t) path[i]]->server = 0;
			}
			root = root->next[(size_t) path[i]];
		}
	}

	root->server = server;
	root->inode  = inode;
}

struct vfs *vfs_get(struct vfs *root, const char *path) {
	size_t i;

	for (i = 0; path[i]; i++) {
		if (root && root->link && path[i] == '/' || path[i] == '\0') {
			root = root->link;
		}
		else {
			if (!root || !root->next) {
				return NULL;
			}
			root = root->next[(size_t) path[i]];
		}
	}

	return root;
}

void  vfs_lnk(struct vfs *root, const char *path, struct vfs *link) {
	size_t i;

	for (i = 0; path[i]; i++) {
		if (root->link && path[i] == '/' || path[i] == '\0') {
			root = root->link;
		}
		else {
			if (!root || !root->next && !root->link) {
				return;
			}
			root = root->next[(size_t) path[i]];
		}
	}

	root->link = link;
}

static void vfs_reclist(struct vfs *root, char *buffer, char *path) {
	char *pathbuffer, m[2];
	size_t i;

	if (!root) {
		return;
	}

	if (root->server != 0) {
		if (buffer[0]) strcat(buffer, " ");
		strcat(buffer, path);
	}

	pathbuffer = malloc(100);
	m[1] = '\0';

	if (root->next) for (i = 0; i < 128; i++) {
		if (i == '/') continue;
		if (root->next[i]) {
			strcpy(pathbuffer, path);
			m[0] = (char) i;
			strcat(pathbuffer, m);
			vfs_reclist(root->next[i], buffer, pathbuffer);
		}
	}

	free(pathbuffer);
}

void vfs_list(struct vfs *root, const char *path, char *buffer) {
	size_t i;

	strcpy(buffer, "");

	for (i = 0; path[i]; i++) {
		if (root->link && path[i] == '/' || path[i] == '\0') {
			root = root->link;
		}
		else {
			if (!root || !root->next && !root->link) {
				return;
			}
			root = root->next[(size_t) path[i]];
		}
	}

	if (!path[0]) {
		vfs_reclist(root, buffer, "");
	}
	else if (root->next && root->next['/']) {
		vfs_reclist(root->next['/'], buffer, "");
	}
}
