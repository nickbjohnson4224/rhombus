/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/heap.h>
#include <flux/io.h>

#include <stdlib.h>

static int _atoi(const char *nptr) {
	int i, sum;

	for (sum = 0, i = 0; nptr[i] >= '0' && nptr[i] <= '9'; i++) {
		sum *= 10;
		sum += nptr[i] - '0';
	}

	return sum;
}

int find(const char *path) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_FIND;
	arch_strcpy(q.path0, path);

	i = query(FD_STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	return fdsetup(q.server, q.inode);
}

int fadd(const char *path, uint32_t server, uint64_t inode) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_ADD;
	arch_strcpy(q.path0, path);

	q.server = server;
	q.inode  = inode;

	i = query(FD_STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	return 0;
}

int list(const char *path, char *buffer) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_LIST;
	arch_strcpy(q.path0, path);

	i = query(FD_STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	arch_strcpy(buffer, q.path1);

	return 0;
}
