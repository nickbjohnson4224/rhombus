#ifndef VFSD_H
#define VFSD_H

#include <stdint.h>

struct vfs {
	struct vfs **next;
	struct vfs *link;

	uint32_t server;
	uint64_t inode;
};

struct vfs *vfs_get(struct vfs *root, const char *path);
void vfs_add(struct vfs *root, const char *path, uint32_t server, uint64_t inode);
void vfs_lnk(struct vfs *root, const char *path, struct vfs *link);
bool vfs_rem(struct vfs *root, const char *path);

#endif/*VFSD_H*/
