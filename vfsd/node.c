/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <string.h>
#include <vfsd.h>

node_t *vfs_node_new(char *name, server_t *server, uint64_t inode, int type) {
	node_t *node;

	node = malloc(sizeof(node_t));

	strcpy(node->name, name);

	node->server = server;
	node->child  = NULL;
	node->sister = NULL;
	node->parent = NULL;
	node->type   = type;
	node->inode  = inode;

	return node;
}

node_t *vfs_node_add(node_t *node, node_t *child) {
	
	child->parent = node;
	child->sister = node->child;
	node->child   = child;

	return node;
}

node_t *vfs_node_find(node_t *node, char *name) {
	node_t *child;

	child = node->child;

	while (child) {
		if (!strcmp(name, child->name)) {
			return child;
		}

		child = child->sister;
	}

	return NULL;
}
