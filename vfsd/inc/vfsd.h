#ifndef VFSD_H
#define VFSD_H

#include <flux/arch.h>

#define TYPE_FILE 0
#define TYPE_DIR  1
#define TYPE_MNT  2
#define TYPE_LINK 3
#define TYPE_NULL 4

struct vfs_node {
	char name[100];
	
	uint32_t type;

	uint32_t number;
	struct vfs_node *server;

	struct vfs_node *child;
	struct vfs_node *sister;
	struct vfs_node *parent;
};

extern struct vfs_node *vfs;

char **path_parse(const char *path);

struct vfs_node *vfs_node_new (const char *name, int type);
struct vfs_node *vfs_node_add (struct vfs_node *node, struct vfs_node *child);
struct vfs_node *vfs_node_find(struct vfs_node *node, const char *child_name);

struct vfs_node *vfs_find (const char *path);
struct vfs_node *vfs_add  (const char *path, const char *server, uint32_t inode);
struct vfs_node *vfs_del  (const char *path);
struct vfs_node *vfs_mkdir(const char *path);
struct vfs_node *vfs_touch(const char *path);
struct vfs_node *vfs_mount(const char *path, const char *server);

#endif/*VFSD_H*/
