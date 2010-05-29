/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/abi.h>
#include <flux/ipc.h>
#include <flux/heap.h>
#include <flux/mmap.h>

/****************************************************************************
 * event_handler
 *
 * Array of event handlers called by _on_event, corresponding to each port.
 */

static volatile event_t event_handler[256];

/****************************************************************************
 * send
 *
 * Sends a packet to the specified port of the specified target process.
 * The given packet is freed.
 */

uint32_t send(uint32_t port, uint32_t target, struct packet *packet) {
	uint32_t err;

	err = _send(port, target, packet);

	if (!err && packet) {
		packet_free(packet);
	}

	return err;
}

/****************************************************************************
 * recvs
 *
 * Recieves a packet (if one exists) from the specified port that originated
 * from the pid <source>.
 */

struct packet *recvs(uint32_t port, uint32_t source) {
	uintptr_t packet_size;
	struct packet *packet;

	packet_size = _recv(port, source);

	if (packet_size == (uintptr_t) -1 || packet_size == 0) {
		return NULL;
	}

	packet = packet_alloc(packet_size * PAGESZ);
	_pack((uintptr_t) packet);

	return packet;
}

/****************************************************************************
 * recv
 *
 * Recieves a packet (if one exists) from the specified port.
 */

struct packet *recv(uint32_t port) {
	return recvs(port, 0);
}

/****************************************************************************
 * waits
 *
 * Recieves a packet (or waits if one does not yet exist) from the specified
 * port that originated from the pid <source>.
 */

struct packet *waits(uint32_t port, uint32_t source) {
	uintptr_t packet_size;
	struct packet *packet;

	packet = NULL;

	do {
		packet_size = _recv(port, source);

		if (packet_size == (uintptr_t) -1) {
			continue;
		}

		if (packet_size > 0) {
			packet = packet_alloc(packet_size * PAGESZ);
			_pack((uintptr_t) packet);
		}

		break;
	} while (1);

	return packet;
}

/****************************************************************************
 * wait
 *
 * Recieves a packet (or waits if one does not yet exist) from the specified
 * port.
 */

struct packet *wait(uint32_t port) {
	return waits(port, 0);
}

/***************************************************************************
 * on_event
 *
 * Called by _on_event (io/_on_event.s). Redirects event to proper event
 * handler.
 */

void on_event(uint32_t port, uint32_t source) {
	struct packet *packet;

	packet = recvs(port, source);

	event_handler[port](source, packet);
}

/****************************************************************************
 * when
 *
 * Registers a handler to be called on reception of a packet into a specific
 * port. If the handler is NULL, no handler is called on reception of the
 * packet; the packet is instead queued in the port. Returns previously
 * registered handler.
 */

event_t when(uint32_t port, event_t handler) {
	extern void _on_event(void);
	event_t old_handler;

	old_handler = event_handler[port];
	event_handler[port] = handler;

	if (handler) {
		_when(port, (uintptr_t) _on_event);
	}
	else {
		_when(port, (uintptr_t) NULL);
	}

	return old_handler;
}
