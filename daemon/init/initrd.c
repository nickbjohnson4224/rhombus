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

#include <mutex.h>
#include <ipc.h>
#include <proc.h>

#include <stdio.h>
#include <natio.h>
#include <string.h>

#include "inc/tar.h"

static uint8_t *initrd;
static size_t   initrd_size;
static bool   m_initrd;

static void initrd_read(struct packet *packet, uint8_t port, uint32_t caller) {
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
	memcpy(pgetbuf(packet), &initrd[offset], packet->data_length);
	mutex_free(&m_initrd);

	psend(PORT_REPLY, caller, packet);
}

void initrd_init() {
	initrd = (uint8_t*) BOOT_IMAGE;
	initrd_size = tar_size(initrd);

	vfctrll("/dev/initrd", "size", "%d", initrd_size);

	when(PORT_READ, initrd_read);
}
