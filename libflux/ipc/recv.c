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
 * _recvm
 */

static struct packet *_recvm
	(uint8_t port, uint32_t source, uint64_t inode, uint16_t id, uint16_t frag) {
	struct message *m;
	struct packet *packet;
	bool match;

	mutex_spin(&m_msg_queue[port]);
	m = msg_queue[port].next;
	
	while (m) {
		match = true;

		if (source) {
			if (source != m->source) {
				match = false;
			}
		}

		if (inode) {
			if (!m->packet || inode != m->packet->source_inode) {
				match = false;
			}
		}

		if (id) {
			if (!m->packet || id != m->packet->identity) {
				match = false;
			}
		}

		if (frag != 0xFFFF) {
			if (!m->packet || frag != m->packet->fragment_index) {
				match = false;
			}
		}

			if (match == true) break;
			m = m->next;
	}

	if (match) {
		if (m->prev) m->prev->next = m->next;
		if (m->next) m->next->prev = m->prev;

		packet = m->packet;
		heap_free(m);
	}
	else {
		packet = NULL;
	}

	mutex_free(&m_msg_queue[port]);

	return packet;
}

/****************************************************************************
 * recv
 */

struct packet *recv(uint8_t port) {
	return _recvm(port, 0, 0, 0, -1);
}

/****************************************************************************
 * recvs
 */

struct packet *recvs(uint8_t port, uint32_t source) {
	return _recvm(port, source, 0, 0, -1);
}

/****************************************************************************
 * recvn
 */

struct packet *recvn(uint8_t port, uint32_t source, uint64_t inode) {
	return _recvm(port, source, inode, 0, -1);
}

/****************************************************************************
 * recvi
 */

struct packet *recvi(uint8_t port, uint32_t source, uint16_t id) {
	return _recvm(port, source, 0, id, -1);
}

/****************************************************************************
 * recvf
 */

struct packet *recvf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag) {
	return _recvm(port, source, 0, id, frag);
}
