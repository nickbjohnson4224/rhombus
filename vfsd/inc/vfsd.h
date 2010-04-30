#ifndef VFSD_H
#define VFSD_H

#include <stdint.h>

#define TYPE_FILE 0
#define TYPE_DIR  1
#define TYPE_LINK 2
#define TYPE_NULL 3

typedef struct vfs_node {
	char name[100];
	
	uint32_t type;
	uint64_t inode;
	struct vfs_server *server;

	struct vfs_node *child;
	struct vfs_node *sister;
	struct vfs_node *parent;
} node_t;

typedef struct vfs_server {
	uint32_t mutex;

	uint32_t target;

	struct vfs_node *root;
	struct vfs_server *next;
} server_t;

char **vfs_parse(char *path);

node_t *vfs_node_new (char *name, server_t *server, uint64_t inode, int type);
node_t *vfs_node_add (node_t *node, node_t *child);
node_t *vfs_node_find(node_t *node, char *name);

node_t *vfs_tree_find(node_t *tree, char **pathv);
node_t *vfs_tree_add (node_t *tree, node_t *node, char **pathv);

node_t *vfs_find(char *path);
node_t *vfs_add (char *path, node_t *node);

extern server_t *servers[256];

server_t *vfs_get_server(uint32_t server);
server_t *vfs_add_server(uint32_t server, char *name);

#endif/*VFSD_H*/
