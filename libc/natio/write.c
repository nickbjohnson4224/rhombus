/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

/****************************************************************************
 * write
 *
 * Write <size> bytes from <buf> to offset <offset> in file <file>. Returns 
 * the number of bytes written.
 *
 * protocol:
 *   port: PORT_WRITE
 *
 *   request:
 *     uint64_t offset
 *     uint8_t data[]
 *
 *   reply:
 *     uint32_t size
 */

size_t write(uint64_t file, void *buf, size_t size, uint64_t offset) {
	struct msg *msg;

	msg = aalloc(sizeof(struct msg) + sizeof(uint64_t) + size, PAGESZ);
	msg->source = RP_CONS(getpid(), 0);
	msg->target = file;
	msg->length = sizeof(uint64_t) + size;
	msg->port   = PORT_WRITE;
	msg->arch   = ARCH_NAT;
	((uint64_t*) msg->data)[0] = offset;
	memcpy(&msg->data[sizeof(uint64_t)], buf, size);

	if (msend(msg)) return 0;
	msg = mwait(PORT_REPLY, file);

	if (msg->length != sizeof(uint32_t)) {
		size = 0;
	}
	else {
		size = ((uint32_t*) msg->data)[0];
	}

	free(msg);
	return size;
}
