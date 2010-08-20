/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <ipc.h>

#include <stdio.h>
#include <stdlib.h>
#include <mutex.h>

#include "tarfs.h"

/****************************************************************************
 * tarfs_read
 *
 * PORT_READ handler.
 */

void tarfs_read(struct packet *packet, uint8_t port, uint32_t caller) {
	uint32_t offset;
	uint32_t i;
	size_t size;

	i = packet->target_inode;

	/* reject requests to nonexistent inodes */
	if (i > 256 || inode[i].name[0] == '\0') {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	/* get request information */
	offset = packet->offset + inode[i].offset;
	size   = packet->data_length;

	/* bounds check size */
	if (packet->offset + size > inode[i].size) {
		size = inode[i].size - packet->offset;
		psetbuf(&packet, size);
	}

	mutex_spin(&m_parent);
	fseek(parent, offset, SEEK_SET);
	fread(pgetbuf(packet), size, 1, parent);
	mutex_free(&m_parent);

	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}
