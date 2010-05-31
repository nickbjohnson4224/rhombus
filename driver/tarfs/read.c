/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/io.h>

#include <stdio.h>
#include <stdlib.h>

#include "tarfs.h"

/****************************************************************************
 * tarfs_read
 *
 * PORT_READ handler.
 */

void tarfs_read(uint32_t source, struct packet *packet) {
	uint32_t offset;
	uint32_t i;
	size_t size;

	i = packet->target_inode;

	/* reject requests to nonexistent inodes */
	if (i > 256 || inode[i].name[0] == '\0') {
		packet_free(packet);
		send(PORT_REPLY, source, NULL);
		return;
	}

	/* get request information */
	offset = packet->offset + inode[i].offset;
	size   = packet->data_length;

	/* bounds check size */
	if (packet->offset + size > inode[i].size) {
		size = inode[i].size - packet->offset;
		packet_setbuf(&packet, size);
	}

	mutex_spin(&m_parent);
	fseek(parent, offset, SEEK_SET);
	fread(packet_getbuf(packet), size, 1, parent);
	mutex_free(&m_parent);

	send(PORT_REPLY, source, packet);
	packet_free(packet);
}
