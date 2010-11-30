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

#include <stdlib.h>
#include <mutex.h>
#include <page.h>
#include <abi.h>
#include <ipc.h>

/****************************************************************************
 * event_handler
 *
 * Array of event handlers called by _on_event, corresponding to each port.
 */

event_t  event_handler[256];
bool   m_event_handler;

/***************************************************************************
 * on_event
 *
 * Called by _on_event (io/_on_event.s). Receives the message from the 
 * virtual packet register and redirects the event to proper event handler.
 */

void on_event(void) {
	struct packet *packet;
	uint32_t source, port;
	size_t length;

	length = _gvpr(0, VPR_LENGTH);

	if (length) {
		packet = palloc(length);
		page_pack(packet, length, PROT_READ | PROT_WRITE);
	}
	else {
		packet = NULL;
	}

	port   = _gvpr(0, VPR_PORT);
	source = _gvpr(0, VPR_SOURCE);

	mutex_spin(&m_event_handler);

	if (event_handler[port]) {
		event_handler[port](packet, port, source);
	}
	else {
		pstash(packet, port, source);
	}

	mutex_free(&m_event_handler);
}

/****************************************************************************
 * when
 *
 * Registers a handler to be called on reception of a packet into a specific
 * port. If the handler is NULL, no handler is called on reception of the
 * packet; the packet is instead queued in the port. Returns previously
 * registered handler.
 */

static volatile uintptr_t thingy;

event_t when(uint8_t port, event_t handler) {
	extern void _on_event(void);
	event_t old_handler;

	/* do not delete - keeps _on_event linked */
	thingy = (uintptr_t) _on_event;

	mutex_spin(&m_event_handler);

	old_handler = event_handler[port];
	event_handler[port] = handler;

	mutex_free(&m_event_handler);

	return old_handler;
}
