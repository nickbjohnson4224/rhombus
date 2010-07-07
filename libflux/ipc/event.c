/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/abi.h>
#include <flux/ipc.h>
#include <flux/heap.h>

/****************************************************************************
 * event_handler
 *
 * Array of event handlers called by _on_event, corresponding to each port.
 */

event_t  event_handler[256];
uint32_t m_event_handler;

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
		packet = packet_alloc(length);
		_gvpr((uintptr_t) packet, VPR_FRAME);
	}
	else {
		packet = NULL;
	}

	port   = _gvpr(0, VPR_PORT);
	source = _gvpr(0, VPR_SOURCE);

	mutex_spin(&m_event_handler);

	if (event_handler[port]) {
		event_handler[port](source, packet);
	}
	else {
		stash(packet, port, source);
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
