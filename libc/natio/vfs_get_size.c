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
 * vfs_get_size
 *
 * Finds the size of the file in driver <root> with path <path>. Returns the
 * size on success, zero on failure and character devices.
 */

uint64_t vfs_get_size(FILE *root, const char *path) {
	struct vfs_query query;
	uint64_t size;

	query.opcode = VFS_ACT | VFS_GET | VFS_SIZE;
	strlcpy(query.path0, path, MAX_PATH);

	if (!vfssend(root, &query)) {
		return 0;
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
			return 0;
		}
		else {
			size  = query.value0;
			size += (uint64_t) query.value1 << 32;
			return size;
		}
	}
}
