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

/****************************************************************************
 * vfs_get_perm
 *
 * Returns the permissions given to user <user> on the file in driver <root>
 * at path <path>.
 */

uint16_t vfs_get_perm(FILE *root, const char *path, uint32_t user) {
	struct vfs_query query;

	query.opcode = VFS_ACT | VFS_GET | VFS_PERM;
	strlcpy(query.path0, path, MAX_PATH);
	query.value0 = user;

	if (!vfssend(root, &query)) {
		return 0;
	}
	else {
		if (query.opcode & VFS_ERR) {
			return 0;
		}
		else {
			return query.opcode;
		}
	}
}
