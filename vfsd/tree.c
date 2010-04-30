/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for detailed
 */

#include <stdlib.h>
#include <stdio.h>
#include <vfsd.h>

node_t *vfs_tree_find(node_t *tree, char **pathv) {
	
	if (tree == NULL) {
		return NULL;
	}

	if (pathv[0] == NULL) {
		return tree;
	}

	return vfs_tree_find(vfs_node_find(tree, pathv[0]), &pathv[1]);
}

node_t *vfs_tree_add(node_t *tree, node_t *node, char **pathv) {
	
	if (tree == NULL) {
		return NULL;
	}

	if (pathv[0] == NULL) {
		vfs_node_add(tree, node);
		return tree;
	}

	return vfs_tree_add(vfs_node_find(tree, pathv[0]), node, &pathv[1]);
}
