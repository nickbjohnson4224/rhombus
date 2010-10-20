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
#include <stdio.h>
#include <natio.h>
#include <proc.h>

void lfs_new_default(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *node, *dir;

	dir = lfs_get_dir(lfs_get_node(inode), query->path0);
	
	if (!dir) {
		query->opcode = VFS_ERR | VFS_DIR;
		return;
	}

	if (!drv_new) {
		query->opcode = VFS_ERR | VFS_PERM;
		return;
	}

	node = drv_new(query, dir);

	if (!node) {
		return;
	}

	lfs_add_path(lfs_get_node(inode), query->path0, node);
	lfs_add_node(node);

	query->file0[0] = getpid();
	query->file0[1] = node->inode;
}

void lfs_del_default(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *node;

	node = lfs_get_path(lfs_get_node(inode), query->path0);
	
	if (!node) {
		query->opcode = VFS_ERR | VFS_FILE;
		return;
	}

	if ((lfs_get_perm(node, 0) & PERM_DEL) == 0) {
		query->opcode = VFS_ERR | VFS_PERM;
		return;
	}

	if (!drv_del) {
		query->opcode = VFS_ERR | VFS_PERM;
		return;
	}

	node = lfs_del_path(lfs_get_node(inode), query->path0);
	lfs_del_node(node->inode);
	drv_del(query, node);
}

void lfs_get_default(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *node;
	
	node = lfs_get_path(lfs_get_node(inode), query->path0);

	if (!node) {
		query->opcode = VFS_ERR | VFS_GET | VFS_FILE;
		return;
	}

	if (!(lfs_get_perm(node, 0) & PERM_GET)) {
		query->opcode = VFS_ERR | VFS_GET | VFS_PERM;
		return;
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
		if (node->type != VFS_DIR) {
			query->opcode = VFS_ERR | VFS_TYPE;
		}
		else {
			lfs_list_dir(query->path0, MAX_PATH, node);
		}

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
	case VFS_PERM:
		if (query->value0 == node->user) {
			query->value0 = node->perm_user;
		}
		else {
			query->value0 = node->perm_def;
		}
		break;
	deafult:
		query->opcode = VFS_ERR | VFS_NOUN;
		break;
	}
}

void lfs_set_default(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *node;

	node = lfs_get_path(lfs_get_node(inode), query->path0);

	if (!node) {
		query->opcode = VFS_ERR | VFS_GET | VFS_FILE;
		return;
	}

	switch (query->opcode & VFS_NOUN) {
	case VFS_LINK:
		if (!(lfs_get_perm(node, 0) & PERM_LINK)) {
			query->opcode = VFS_ERR | VFS_SET | VFS_PERM;
			return;
		}
		node->link  = strdup(query->path0);
		node->alink = __fcons(query->file0[0], query->file0[1], NULL);
		break;
	case VFS_PERM:
		if (!(lfs_get_perm(node, 0) & PERM_PERM)) {
			query->opcode = VFS_ERR | VFS_SET | VFS_PERM;
			return;
		}
		query->opcode = lfs_set_perm(node, query->value0, query->value1);
		break;
	default:
		query->opcode = VFS_ERR | VFS_NOUN;
		break;
	}

	if (drv_set) {
		drv_set(query, node);
	}
}
