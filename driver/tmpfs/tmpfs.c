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

#include "tmpfs.h"

struct  tmpfs_inode tmpfs_inode_table[256];
uint8_t tmpfs_inode_top = 1;

static struct tmpfs_inode *tmpfs_new_inode(void) {
	struct tmpfs_inode *inode;

	inode = &tmpfs_inode_table[tmpfs_inode_top];
	inode->inode = tmpfs_inode_top;
	tmpfs_inode_top++;

	return inode;
}

void tmpfs_new(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *node, *root;
	struct tmpfs_inode *tmpnode;

	switch (query->opcode & VFS_NOUN) {
	case VFS_FILE:
		tmpnode = tmpfs_new_inode();
		root = lfs_get_node(inode);
		node = lfs_new_file(tmpnode->inode, 0);
		lfs_add_path(root, query->path0, node);
		lfs_add_node(node);
		lfs_set_perm(node, 0, PERM_GET | PERM_PERM | PERM_READ | PERM_WRITE);
		break;
	case VFS_DIR:
		tmpnode = tmpfs_new_inode();
		root = lfs_get_node(inode);
		node = lfs_new_dir(tmpnode->inode);
		lfs_add_path(root, query->path0, node);
		lfs_add_node(node);
		lfs_set_perm(node, 0, PERM_GET | PERM_PERM | PERM_NEW | PERM_DEL);
		break;
	default:
		query->opcode = VFS_ERR | VFS_PERM;
		break;
	}
}

void tmpfs_del(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *root, *node;

	root = lfs_get_node(inode);
	node = lfs_del_path(root, query->path0);
	
	if (!node) {
		query->opcode = VFS_ERR | VFS_FILE;
		return;
	}

	lfs_del_node(node->inode);
	free(node);
}
	
void tmpfs_mov(struct vfs_query *query, uint32_t inode, uint32_t caller) {
	struct lfs_node *root, *node;

	root = lfs_get_node(inode);
	node = lfs_del_path(root, query->path0);

	if (!node) {
		query->opcode = VFS_ERR | VFS_FILE;
		return;
	}

	if (lfs_add_path(root, query->path1, node)) {
		lfs_add_path(root, query->path0, node);
		query->opcode = VFS_ERR | VFS_PATH;
		return;
	}
}
	

void tmpfs_read(struct packet *packet, uint8_t port, uint32_t caller) {
	struct tmpfs_inode *inode;
	char *buffer;
	size_t size;

	if (!packet) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	inode = &tmpfs_inode_table[packet->target_inode];
	
	if (packet->offset < inode->size) {
		buffer = pgetbuf(packet);
		size = packet->data_length;
		size = ((inode->size - packet->offset) < size) ? (inode->size - packet->offset) : size;
		psetbuf(&packet, size);
		memcpy(buffer, &inode->data[packet->offset], size);
	}
	else {
		psetbuf(&packet, 0);
	}
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

void tmpfs_write(struct packet *packet, uint8_t port, uint32_t caller) {
	struct tmpfs_inode *inode;
	char *buffer;

	if (!packet) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}
	
	buffer = pgetbuf(packet);
	inode = &tmpfs_inode_table[packet->target_inode];
	
	if (packet->offset + packet->data_length > inode->size) {
		inode->data = realloc(inode->data, packet->offset + packet->data_length);
		inode->size = packet->offset + packet->data_length;
	}

	memcpy(&inode->data[packet->offset], buffer, packet->data_length);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

void tmpfs_init(void) {

	lfs_set_perm(lfs_get_node(0), 0, PERM_GET | PERM_PERM | PERM_NEW | PERM_DEL);

	when(PORT_READ, tmpfs_read);
	when(PORT_WRITE, tmpfs_write);

	lfs_when_new(tmpfs_new);
	lfs_when_del(tmpfs_del);
	lfs_when_mov(tmpfs_mov);
	lfs_event_start();

}
