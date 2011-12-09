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

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/ipc.h>

/****************************************************************************
 * read
 *
 * Read <size> bytes into <buf> from offset <offset> in file <file>. Returns 
 * the number of bytes read.
 *
 * protocol:
 *   port: PORT_READ
 *
 *   request:
 *     uint64_t offset
 *     uint32_t size
 *
 *   reply:
 *     uint8_t data[]
 */

size_t rp_read(uint64_t file, void *buf, size_t size, uint64_t offset) {
	struct msg *msg;

	msg = aalloc(sizeof(struct msg) + sizeof(uint64_t) + sizeof(uint32_t), PAGESZ);
	if (!msg) return 0;
	msg->source = RP_CURRENT_THREAD;
	msg->target = file;
	msg->length = sizeof(uint64_t) + sizeof(uint32_t);
	msg->port   = PORT_READ;
	msg->arch   = ARCH_NAT;
	((uint64_t*) msg->data)[0] = offset;
	((uint32_t*) msg->data)[2] = size;

	if (msend(msg)) return 0;
	msg = mwait(PORT_REPLY, file);

	if (size > msg->length) {
		size = msg->length;
	}

	if (size) memcpy(buf, msg->data, size);

	free(msg);
	return size;
}

int __zab(int x) {
	return x + 1;
}
