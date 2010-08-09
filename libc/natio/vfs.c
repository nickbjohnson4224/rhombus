/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <natio.h>
#include <dict.h>

/****************************************************************************
 * find
 *
 * Finds the server and inode of a file given its path. On success, returns
 * zero and sets *<server> to the server and *<inode> to the inode. On 
 * failure, returns nonzero.
 */

int find(const char *path, uint32_t *server, uint64_t *inode) {
	char *value;

	value = dreadns("vfs:", path);

	if (!value) {
		return -1;
	}

	*server = atoi(strtok(value, ":"));
	*inode  = atoi(strtok(NULL, ":"));

	free(value);

	return 0;
}

/****************************************************************************
 * fadd (DEPRECATED)
 *
 * Adds a file to the VFS pointing to <server> and <inode>. Returns 0 on
 * success, nonzero on failure.
 */

int fadd(const char *path, uint32_t server, uint64_t inode) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_ADD;
	strcpy(q.path0, path);

	q.server = server;
	q.inode  = inode;

	i = query(stdvfs, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	return 0;
}

/****************************************************************************
 * list
 *
 * Finds the contents of the directory at <path> in the VFS. On success,
 * returns zero and copies the contents into <buffer>. On failure, returns
 * nonzero.
 */

int list(const char *path, char *buffer) {
	struct vfs_query q;
	size_t i;

	q.command = VFS_CMD_LIST;
	strcpy(q.path0, path);

	i = query(stdvfs, &q, &q, sizeof(struct vfs_query));

	if (i == 0) {
		return -1;
	}

	strcpy(buffer, q.path1);

	return 0;
}
