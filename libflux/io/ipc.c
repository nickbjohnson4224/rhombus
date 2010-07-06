/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/abi.h>
#include <flux/ipc.h>
#include <flux/heap.h>
#include <flux/mmap.h>
#include <flux/proc.h>

/****************************************************************************
 * _packet_queue
 *
 * Structure used to queue packets.
 */

static struct _packet_queue {
	struct _packet_queue *next;
	struct _packet_queue *prev;
	struct packet *packet;
	uint32_t source;
} _packet_queue[256];

uint32_t m_packet_queue;

/****************************************************************************
 * _queue
 *
 * Queues a packet based on its contents.
 */

static void _queue(struct packet *packet, uint32_t source, uint8_t port) {
	struct _packet_queue *pq;

	pq = heap_malloc(sizeof(struct _packet_queue));
	pq->packet = packet;
	pq->source = source;

	mutex_spin(&m_packet_queue);

	pq->prev = &_packet_queue[port];
	pq->next =  _packet_queue[port].next;
	_packet_queue[port].next = pq;

	mutex_free(&m_packet_queue);
}

/****************************************************************************
 * _waitm
 */

struct packet *_waitm(uint8_t port, uint32_t source) {
	struct _packet_queue *pq;
	struct packet *packet;
	bool match;
	int i;

	do {
		mutex_spin(&m_packet_queue);

		pq = _packet_queue[port].next;

		while (pq) {
			match = true;

			if (source && source != pq->source) {
				match = false;
			}

			if (match == true) break;
			pq = pq->next;
		}

		if (pq) {
			if (pq->next) pq->next->prev = pq->prev;
			if (pq->prev) pq->prev->next = pq->next;

			packet = pq->packet;
			heap_free(pq);
			break;
		}

		mutex_free(&m_packet_queue);

	} while (1);

	return packet;
}

/****************************************************************************
 * _recvm
 */

static struct packet *_recvm(uint8_t port, uint32_t source) {
	struct _packet_queue *pq;
	struct packet *packet;
	bool match;

	mutex_spin(&m_packet_queue);

	pq = _packet_queue[port].next;
	
	while (pq) {
		match = true;

		if (source && source != pq->source) {
			match = false;
		}

		if (match == true) break;
	
		pq = pq->next;
	}

	if (match) {
		if (pq->prev) pq->prev->next = pq->next;
		if (pq->next) pq->next->prev = pq->prev;

		packet = pq->packet;
		heap_free((void*) pq);
	}
	else {
		packet = NULL;
	}

	mutex_free(&m_packet_queue);

	return packet;
}

/****************************************************************************
 * event_handler
 *
 * Array of event handlers called by _on_event, corresponding to each port.
 */

static volatile event_t event_handler[256];

/****************************************************************************
 * setpacket
 *
 * Sets the current thread's virtual packet register to the given packet.
 * The packet's virtual memory is freed.
 */

void setpacket(struct packet *packet) {
	_svpr((uintptr_t) packet, 0);
	packet_free(packet);
}

/****************************************************************************
 * getpacket
 *
 * Gets the packet contained in the current thread's virtual packet register,
 * allocates virtual memory for it, and returns it. Returns NULL if the
 * packet is of size zero.
 */

struct packet *getpacket(void) {
	struct packet *packet;
	size_t length;

	length = _gvpr(0, VPR_LENGTH);

	if (length) {
		packet = packet_alloc(PAGESZ);
		_gvpr((uintptr_t) packet, VPR_FRAME);

		return packet;
	}
	else {
		return NULL;
	}
}

/****************************************************************************
 * send
 *
 * Sends a packet to the specified port of the specified target process.
 * The given packet is freed.
 */

uint32_t send(uint32_t port, uint32_t target, struct packet *packet) {
	uint32_t err;

	setpacket(packet);
	err = _send(target, port);

	return err;
}

/***************************************************************************
 * on_event
 *
 * Called by _on_event (io/_on_event.s). Redirects event to proper event
 * handler.
 */

void on_event(void) {
	struct packet *packet;
	uint32_t source, port;

	packet = getpacket();
	port   = _gvpr(0, VPR_PORT);
	source = _gvpr(0, VPR_SOURCE);

	if (event_handler[port]) {
		event_handler[port](source, packet);
	}
	else {
		_queue(packet, source, port);
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

event_t when(uint32_t port, event_t handler) {
	extern void _on_event(void);
	event_t old_handler;

	/* do not delete - keeps _on_event linked */
	thingy = (uintptr_t) _on_event;

	old_handler = event_handler[port];
	event_handler[port] = handler;

	return old_handler;
}

/****************************************************************************
 * recvs
 */

struct packet *recvs(uint32_t port, uint32_t source) {
	return _recvm(port, source);
}

/****************************************************************************
 * recv
 */

struct packet *recv(uint32_t port) {
	return _recvm(port, 0);
}

/****************************************************************************
 * waits
 */

struct packet *waits(uint32_t port, uint32_t source) {
	return _waitm(port, source);
}

/****************************************************************************
 * wait
 */

struct packet *wait(uint32_t port) {
	return _waitm(port, 0);
}
