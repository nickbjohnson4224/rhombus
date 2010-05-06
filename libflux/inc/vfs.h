#ifndef FLUX_VFS_H
#define FLUX_VFS_H

#include <flux/arch.h>

#define VFS_CMD_FIND  0
#define VFS_CMD_ADD   1

#define VFS_CMD_REPLY 3
#define VFS_CMD_ERROR 4

struct vfs_query {
	uint32_t command;
	uint32_t server;
	uint64_t inode;
	char path0[1000];
	char path1[1000];
};

int find(const char *path);
int open(const char *path);

#endif/*FLUX_VFS_H*/
