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
 * fs_remove
 *
 * Attempts to remove the fileystem object <fobj>. Returns zero on success,
 * nonzero on failure.
 *
 * protocol:
 *   port: PORT_REMV
 *
 *   request:
 *   
 *   reply:
 *     uint8_t err;
 */

int fs_remove(uint64_t fobj) {
	struct msg *msg;
	int err;

	msg = aalloc(sizeof(struct msg), PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = fobj;
	msg->length = 0;
	msg->port   = PORT_REMV;
	msg->arch   = ARCH_NAT;

	if (msend(msg)) return 1;
	msg = mwait(PORT_REPLY, fobj);

	if (msg->length < sizeof(uint8_t)) {
		free(msg);
		return 1;
	}

	err = msg->data[0];

	free(msg);
	return err;
}
