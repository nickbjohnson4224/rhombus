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
#include <natio.h>
#include <errno.h>

/****************************************************************************
 * vfs_new_link
 *
 * Sends a request for a new symbolic link to be created in driver <root>
 * at path <path> to path <link>. Returns a file descriptor of the new link 
 * on success, NULL on failure.
 */

FILE *vfs_new_link(FILE *root, const char *path, const char *link, FILE *alink) {
	struct vfs_query query;

	query.opcode = VFS_ACT | VFS_NEW | VFS_LINK;
	strlcpy(query.path0, path, MAX_PATH);

	if (link) {
		strlcpy(query.path1, link, MAX_PATH);
		query.file0[0] = 0;
	}
	else {
		query.file0[0] = alink->server;
		query.file0[1] = alink->inode;
	}

	if (!vfssend(root, &query)) {
		return NULL;
	}
	else {
		if (query.opcode & VFS_ERR) {
			switch (query.opcode & 0x000F) {
			case VFS_FILE:
				errno = EEXIST;
				break;
			case VFS_PERM:
				errno = EPERM;
				break;
			case VFS_LINK:
				errno = EPATH;
				break;
			}
			return NULL;
		}
		else {
			return __fcons(query.file0[0], query.file0[1], NULL);
		}
	}
}
