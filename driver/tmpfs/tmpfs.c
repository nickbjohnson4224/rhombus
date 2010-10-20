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

uint8_t tmpfs_inode_top = 1;

struct lfs_node *tmpfs_new(struct vfs_query *query, struct lfs_node *dir) {
	struct lfs_node *node;

	switch (query->opcode & VFS_NOUN) {
	case VFS_FILE:
		node = lfs_new_file(tmpfs_inode_top++, 0);
		lfs_set_perm(node, 0, PERM_GET | PERM_PERM | PERM_READ | PERM_WRITE | PERM_DEL);
		return node;
	case VFS_DIR:
		node = lfs_new_dir(tmpfs_inode_top++);
		lfs_set_perm(node, 0, PERM_GET | PERM_PERM | PERM_NEW);
		return node;
	case VFS_LINK:
		node = lfs_new_link(query->path1, __fcons(query->file0[0], query->file0[1], NULL));
		lfs_set_perm(node, 0, PERM_GET | PERM_PERM | PERM_DEL);
		return node;
	default:
		query->opcode = VFS_ERR | VFS_PERM;
		return NULL;
	}
}

void tmpfs_del(struct vfs_query *query, struct lfs_node *node) {
	if (node->data) {
		free(node->data);
	}
}

void tmpfs_read(struct packet *packet, uint8_t port, uint32_t caller) {
	struct lfs_node *node;
	char *buffer;
	char *data;
	size_t size;

	if (!packet) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	node = lfs_get_node(packet->target_inode);

	if (!node) {
		psend(PORT_REPLY, caller, NULL);
		pfree(packet);
		return;
	}
	
	if (packet->offset < node->size) {
		buffer = pgetbuf(packet);
		size = packet->data_length;
		size = ((node->size - packet->offset) < size) ? (node->size - packet->offset) : size;
		psetbuf(&packet, size);
		data = node->data;
		memcpy(buffer, &data[packet->offset], size);
	}
	else {
		psetbuf(&packet, 0);
	}
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

void tmpfs_write(struct packet *packet, uint8_t port, uint32_t caller) {
	struct lfs_node *node;
	char *buffer, *data;

	if (!packet) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	node = lfs_get_node(packet->target_inode);

	if (!node) {
		psend(PORT_REPLY, caller, NULL);
		pfree(packet);
		return;
	}

	if (packet->offset + packet->data_length > node->size) {
		node->data = realloc(node->data, packet->offset + packet->data_length);
		node->size = packet->offset + packet->data_length;
	}

	buffer = pgetbuf(packet);
	data   = node->data;

	memcpy(&data[packet->offset], buffer, packet->data_length);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

void tmpfs_init(void) {

	lfs_set_perm(lfs_get_node(0), 0, PERM_GET | PERM_PERM | PERM_NEW | PERM_DEL);

	when(PORT_READ, tmpfs_read);
	when(PORT_WRITE, tmpfs_write);

	drv_new = tmpfs_new;
	drv_del = tmpfs_del;
	lfs_event_start();

}
