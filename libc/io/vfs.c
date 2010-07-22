/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <string.h>
#include <io.h>

int find(const char *path) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_FIND;
	strcpy(q.path0, path);

	i = query(STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	return fdsetup(q.server, q.inode);
}

int fadd(const char *path, uint32_t server, uint64_t inode) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_ADD;
	strcpy(q.path0, path);

	q.server = server;
	q.inode  = inode;

	i = query(STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	return 0;
}

int list(const char *path, char *buffer) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_LIST;
	strcpy(q.path0, path);

	i = query(STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	strcpy(buffer, q.path1);

	return 0;
}
