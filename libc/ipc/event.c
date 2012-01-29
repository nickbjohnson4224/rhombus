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

#include <rho/natio.h>
#include <rho/mutex.h>
#include <rho/page.h>
#include <rho/proc.h>
#include <rho/abi.h>
#include <rho/ipc.h>

/****************************************************************************
 * event_handler
 *
 * Array of event handlers called by _on_event, corresponding to each port.
 */

void (*event_handler[256])(struct msg *msg);
bool m_event_handler;

/***************************************************************************
 * on_event
 *
 * Called by _on_event (io/_on_event.s). Receives the message from the 
 * virtual packet register and redirects the event to proper event handler.
 */

void on_event(size_t count, uint32_t action, uint32_t source, uint32_t source_idx, uint32_t target_idx) {
	struct msg *msg;

	if (count) {
		/* recieve message */
		msg = aalloc(count * PAGESZ, PAGESZ);
		if (!msg) {
			return;
		}

		if (page_pack(msg, count * PAGESZ, PROT_READ | PROT_WRITE) || !phys(msg)) {
			free(msg);
			goto synthesize;
		}

		/* check message contents */
		if (RP_PID(msg->source) != source) {
			free(msg);
			return;
		}

		if (msg->length + sizeof(struct msg) > PAGESZ * count) {
			free(msg);
			return;
		}
	}
	else {
		synthesize:

		/* synthesize message */
		msg = aalloc(sizeof(struct msg), PAGESZ);
		if (!msg) return;

		msg->source = RP_CONS(source, source_idx);
		msg->target = RP_CONS(getpid(), target_idx);
		msg->length = 0;
		msg->action = action;
		msg->arch   = ARCH_NAT;
	}

	if (!msg || !phys(msg)) {
		return;
	}

	mutex_spin(&m_event_handler);
	
	if (event_handler[action]) {
		mutex_free(&m_event_handler);
		event_handler[action](msg);
	}
	else {
		mutex_free(&m_event_handler);
		mqueue_push(msg);
	}
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

void when(uint8_t port, void (*handler)(struct msg *msg)) {
	extern void _on_event(void);

	/* do not delete - keeps _on_event linked */
	thingy = (uintptr_t) _on_event;

	mutex_spin(&m_event_handler); {
		event_handler[port] = handler;
	} mutex_free(&m_event_handler);
}
