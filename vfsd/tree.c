/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for detailed
 */

#include <stdlib.h>
#include <string.h>
#include <vfsd.h>

char **path_parse(const char *path) {
	char **pathv;
	char buffer[100];
	size_t i, j, n, count;

	for (i = 0, count = 0; path[i]; i++) {
		if (path[i] == '/') count++;
	}

	pathv = malloc(sizeof(char*) * count + 2);

	for (i = 0, n = 0, j = 0; path[i]; i++) {
		if (path[i] == '/' || path[i+1] == '\0') {
			buffer[j] = '\0';
			pathv[n] = malloc(strlen(buffer) + 1);
			strcpy(pathv[n], buffer);
			j = 0;
			n++;
		}
		else {
			buffer[j] = path[i];
			j++;
		}
	}

	pathv[n] = NULL;

	return pathv;
}

struct vfs_node *vfs_find(const char *path) {
	char **pathv;
	struct vfs_node *node;
	size_t i;

	pathv = path_parse(path);
	node  = vfs;

	for (i = 0; pathv[i]; i++) {
		node = vfs_node_find(node, pathv[i]);
		free(pathv[i]);

		if (!node) {
			break;
		}
	}

	free(pathv);

	return node;
}
