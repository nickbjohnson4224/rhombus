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
#include <stdlib.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_size
 *
 * Returns the file size of <file>. If this value is zero, the file may not
 * exist, be the wrong type, or be a character device. fs_type can be used to 
 * differentiate between these cases.
 *
 * protocol:
 *   port: PORT_SIZE
 *
 *   request:
 *
 *   reply:
 *     uint64_t size
 */

uint64_t fs_size(uint64_t file) {
	struct msg *msg;
	uint64_t size;

	msg = aalloc(sizeof(struct msg), PAGESZ);
	msg->source = RP_CONS(getpid(), 0);
	msg->target = file;
	msg->length = 0;
	msg->port   = PORT_SIZE;
	msg->arch   = ARCH_NAT;

	if (msend(msg)) return 0;
	msg = mwait(PORT_REPLY, file);

	if (msg->length < sizeof(uint64_t)) {
		free(msg);
		return 0;
	}

	size = ((uint64_t*) msg->data)[0];

	free(msg);
	return size;
}
