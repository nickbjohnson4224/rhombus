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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <natio.h>
#include <stdio.h>
#include <arch.h>
#include <page.h>
#include <ipc.h>

/*****************************************************************************
 * rp_asend
 *
 * Send a message <msg> to the resource <rp> at port <port> without expecting 
 * a reply. <msg>.length must be the total length of the message in bytes. If 
 * <msg> is null, the packet on the message is null. Returns zero on success,
 * nonzero on error. Either way, <msg> is freed.
 */

int rp_asend(uint64_t rp, uint8_t port, struct mp_basic *msg) {
	struct msg *message;
	int err;

	/* set necessary message values */
	if (msg) {
		msg->index = RP_INDEX(rp);
		msg->arch  = MP_ARCH_NATIVE;
		msg->type  = MP_TYPE_ASYNC;
	}

	/* allocate message */
	message = malloc(sizeof(struct msg));

	/* allocate packet */
	if (msg) {
		message->count = (msg->length % PAGESZ) ? (msg->length / PAGESZ) + 1 : msg->length / PAGESZ;
		message->packet = aalloc(message->count * PAGESZ, PAGESZ);

		/* check for out of memory error */
		if (!message->packet) {
			free(msg);
			return 1;
		}

		if ((uintptr_t) msg % PAGESZ) {
			/* unaligned message: copy */
			memcpy(message->packet, msg, msg->length);
		}
		else {
			/* aligned message: move */
			page_self(msg, message->packet, message->count * PAGESZ);
		}

		free(msg);
	}

	/* send message */
	err = msend(port, RP_PID(rp), message);

	if (err) {
		/* free message */
		free(message->packet);
		free(message);
	}

	return err;
}
