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

static event_t event_handler[256];

/****************************************************************************
 * send
 *
 * Sends a packet to the specified port of the specified target process.
 * The given packet is freed.
 */

uint32_t send(uint32_t port, uint32_t target, req_t *request) {
	uint32_t err;

	err = _send(port, target, request);

	if (!err) {
		rfree(request);
	}

	return err;
}

/****************************************************************************
 * recvs
 *
 * Recieves a packet (if one exists) from the specified port that originated
 * from the pid <source>.
 */

struct request *recvs(uint32_t port, uint32_t source) {
	uintptr_t packet_phys;
	struct request *packet;

	packet_phys = _recv(port, 0);

	if (packet_phys & 1) {
		return NULL;
	}

	packet = ralloc();
	emap(packet, packet_phys, PROT_READ | PROT_WRITE);

	return packet;
}

/****************************************************************************
 * recv
 *
 * Recieves a packet (if one exists) from the specified port.
 */

struct request *recv(uint32_t port) {
	return recvs(port, 0);
}

/****************************************************************************
 * waits
 *
 * Recieves a packet (or waits if one does not yet exist) from the specified
 * port that originated from the pid <source>.
 */

struct request *waits(uint32_t port, uint32_t source) {
	struct request *packet;

	packet = NULL;

	while (!packet) {
		packet = recvs(port, source);
	}

	return packet;
}

/****************************************************************************
 * wait
 *
 * Recieves a packet (or waits if one does not yet exist) from the specified
 * port.
 */

struct request *wait(uint32_t port) {
	return waits(port, 0);
}

/***************************************************************************
 * on_event
 *
 * Called by _on_event (io/_on_event.s). Redirects event to proper event
 * handler.
 */

void on_event(uint32_t source, uint32_t port, uintptr_t packet_phys) {
	struct request *packet;

	packet = ralloc();
	emap(packet, packet_phys, PROT_READ | PROT_WRITE);

	event_handler[port](source, packet);
}

/****************************************************************************
 * event
 *
 * Registers a handler to be called on reception of a packet into a specific
 * port. If the handler is NULL, no handler is called on reception of the
 * packet; the packet is instead queued in the port. Returns previously
 * registered handler.
 */

event_t event(uint32_t port, event_t handler) {
	extern void _on_event(void);
	event_t old_handler;

	old_handler = event_handler[port];
	event_handler[port] = handler;

	if (handler) {
		_evnt(port, (uintptr_t) _on_event);
	}
	else {
		_evnt(port, (uintptr_t) NULL);
	}

	return old_handler;
}
