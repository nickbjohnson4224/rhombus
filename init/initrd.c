/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/packet.h>
#include <flux/proc.h>
#include <flux/vfs.h>
#include <flux/io.h>

#include <stdio.h>
#include <string.h>

#include "inc/tar.h"

static uint8_t *initrd;
static size_t   initrd_size;
static uint32_t m_initrd;

static void initrd_read(uint32_t source, struct packet *packet) {
	uintptr_t offset;

	if (!packet) {
		return;
	}

	offset = (uintptr_t) packet->offset;

	if (offset + packet->data_length > initrd_size) {
		if (offset > initrd_size) {
			packet->data_length = 0;
		}
		else {
			packet->data_length = initrd_size - offset;
		}
	}

	mutex_spin(&m_initrd);
	memcpy(packet_getbuf(packet), &initrd[offset], packet->data_length);
	mutex_free(&m_initrd);

	send(PORT_REPLY, source, packet);
}

void initrd_init() {
	initrd = (uint8_t*) BOOT_IMAGE;
	initrd_size = tar_size(initrd);
	when(PORT_READ, initrd_read);
}
