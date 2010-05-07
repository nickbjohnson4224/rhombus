/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/vfs.h>
#include <flux/heap.h>
#include <flux/io.h>

int find(const char *path) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_FIND;
	for (i = 0; path[i]; i++) {
		q.path0[i] = path[i];
	}
	q.path0[i] = '\0';

	i = query(FD_STDVFS, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	return fdsetup(q.server, q.inode);
}
