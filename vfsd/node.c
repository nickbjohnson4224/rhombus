/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <string.h>
#include <vfsd.h>

struct vfs_node *vfs_node_new(const char *name, int type) {
	struct vfs_node *node;

	node = malloc(sizeof(struct vfs_node));

	strcpy(node->name, name);

	node->server = NULL;
	node->child  = NULL;
	node->sister = NULL;
	node->parent = NULL;
	node->type   = type;
	node->number = 0;

	return node;
}

struct vfs_node *vfs_node_add(struct vfs_node *node, struct vfs_node *child) {
	
	child->parent = node;
	child->sister = node->child;
	node->child   = child;

	return node;
}

struct vfs_node *vfs_node_find(struct vfs_node *node, const char *child_name) {
	struct vfs_node *child;

	child = node->child;

	while (child) {
		if (!strcmp(child_name, child->name)) {
			return child;
		}

		child = child->sister;
	}

	return NULL;
}
