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

#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <proc.h>

void lfs_get_default(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *node;
	const char *tail;
	char *tail_copy;
	
	node = lfs_get_path(lfs_get_node(inode), query->path0);

	if (!node || node->type == VFS_LINK) {
		node = lfs_get_link(lfs_get_node(inode), query->path0, &tail);
		if (!node) {
			query->opcode = VFS_ERR | VFS_FILE;
			return;
		}
		else {
			tail_copy = (tail) ? strdup(tail) : strdup("");
			strlcpy(query->path0, node->link, MAX_PATH);
			strlcat(query->path0, "/", MAX_PATH);
			strlcat(query->path0, tail_copy, MAX_PATH);
			free(tail_copy);

			if (node->alink) {
				vfssend((FILE*) node->alink, query);
				return;
			}
		}
	}

	switch (query->opcode & VFS_NOUN) {
	case VFS_TYPE:
		query->value0 = node->type;
		break;
	case VFS_FILE:
		query->file0[0] = getpid();
		query->file0[1] = node->inode;
		break;
	case VFS_DIR:
		lfs_list_dir(query->path0, MAX_PATH, node);
		break;
	case VFS_LINK:
		if (node->type != VFS_LINK) {
			query->opcode = VFS_ERR;
		}
		else {
			strlcpy(query->path0, node->link, MAX_PATH);
		}
		break;
	case VFS_SIZE:
		query->value0 = node->size & 0xFFFFFFFF;
		query->value1 = node->size >> 32;
		break;
	deafult:
		query->opcode = VFS_ERR;
		break;
	}
}
