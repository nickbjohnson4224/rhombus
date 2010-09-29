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
 * vfs_get_file
 *
 * Finds the file descriptor of the file in driver <root> with path <path>.
 * Returns the file descriptor on success, NULL on failure.
 */

FILE *vfs_get_file(FILE *root, const char *path) {
	struct vfs_query query;

	query.opcode = VFS_ACT | VFS_GET | VFS_FILE;
	strlcpy(query.path0, path, MAX_PATH);

	if (!vfssend(root, &query)) {
		return NULL;
	}
	else {
		if (query.opcode & VFS_ERR) {
			switch (query.opcode & VFS_NOUN) {
			case VFS_FILE:
				errno = ENOFILE;
				break;
			case VFS_PERM:
				errno = EPERM;
				break;
			}
			return NULL;
		}
		else {
			return __fcons(query.file0[0], query.file0[1], NULL);
		}
	}
}
