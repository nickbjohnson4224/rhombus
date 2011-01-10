/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <natio.h>
#include <stdio.h>
#include <errno.h>
#include <arch.h>
#include <ipc.h>

/*****************************************************************************
 * rp_tsend
 *
 * Send a message <msg> to the resource <rp> at port <port> and wait for a
 * reply for at most <timeout> timeslices. <msg>.length must be the total 
 * length of the message in bytes. If <msg> is null, the packet on the message 
 * is null. The reply message is returned on success, and null is returned on 
 * failure. Either way, <msg> is freed.
 */

struct msg *rp_tsend(uint64_t rp, uint8_t port, struct mp_basic *msg, uint32_t timeout) {
	struct msg *message;
	event_t old_handler;

	old_handler = when(PORT_REPLY, NULL);
	
	/* set necessary message values */
	if (msg) {
		msg->index = RP_INDEX(rp);
		msg->arch  = MP_ARCH_NATIVE;
		msg->type  = MP_TYPE_SYNC;
	}

	/* allocate message */
	message = malloc(sizeof(struct msg));

	/* allocate packet */
	if (msg) {
		message->count = (msg->length % PAGESZ) ? (msg->length / PAGESZ) + 1 : msg->length / PAGESZ;
		message->packet = aalloc(message->count * PAGESZ, PAGESZ);
		memcpy(message->packet, msg, msg->length);
		free(msg);
	}

	/* send message */
	if (msend(port, RP_PID(rp), message)) {
		/* error sending: abort */
		free(message->packet);
		free(message);

		when(PORT_REPLY, old_handler);
		return NULL;
	}

	/* recieve response */
	message = mwaitst(PORT_REPLY, RP_PID(rp), timeout);

	if (!message) {
		errno = ETIMEDOUT;
	}

	when(PORT_REPLY, old_handler);
	return message;
}
