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
 * vfs_set_perm
 *
 * Sends a request to driver <root> that the permissions of user <user> for 
 * the file at path <path>'s value be changed to <perm>. Returns zero on 
 * success, nonzero on failure.
 */

int vfs_set_perm(FILE *root, const char *path, uint32_t user, uint8_t perm) {
	struct vfs_query query;

	query.opcode = VFS_ACT | VFS_SET | VFS_PERM;
	strlcpy(query.path0, path, MAX_PATH);
	query.value0 = user;
	query.value1 = perm;

	if (!vfssend(root, &query)) {
		return -1;
	}
	else {
		if (query.opcode & VFS_ERR) {
			return query.opcode;
		}
		else {
			return 0;
		}
	}
}
