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
#include <arch.h>
#include <ipc.h>

/***************************************************************************
 * event_send
 *
 * Send an event to the resource <rp> with event id <event_id> and the value
 * <value>. This event is timestamped with the current kernel time. Returns
 * zero on success, nonzero on failure.
 */

int event_send(uint64_t rp, uint32_t event_id, uint32_t value) {
	struct mp_event *event;

	event = malloc(sizeof(struct mp_event));
	event->length    = sizeof(struct mp_event);
	event->protocol  = MP_PROT_EVENT;
	event->event_id  = event_id;
	event->value     = value;
	event->timestamp = 0; /* TODO - actual timestamp */

	return rp_asend(rp, PORT_EVENT, (struct mp_basic*) event);
}
