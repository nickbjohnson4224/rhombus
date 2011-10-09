/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <string.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * event
 *
 * Asynchronous event protocol. Sends an 8-byte value to the given resource,
 * without waiting for a reply. Returns zero on success, nonzero on error.
 */

int event(rp_t rp, const char *value) {
	struct msg *msg;

	if (!value) value = "";

	msg = aalloc(sizeof(struct msg) + strlen(value) + 1, PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = rp;
	msg->length = strlen(value) + 1;
	msg->port   = PORT_EVENT;
	msg->arch   = ARCH_NAT;
	strcpy((char*) msg->data, value);

	return msend(msg);
}

int event_subscribe(rp_t rp) {
	char *reply;

	reply = rcall(rp, "subscribe");

	if (!reply) return 1;
	
	free(reply);
	return 0;
} 

int event_unsubscribe(rp_t rp) {
	char *reply;

	reply = rcall(rp, "unsubscribe");

	if (!reply) return 1;

	free(reply);
	return 0;
}
