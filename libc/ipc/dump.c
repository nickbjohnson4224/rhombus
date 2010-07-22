/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <mutex.h>
#include <ipc.h>

/****************************************************************************
 * _dumpm
 */

static void _dumpm
	(uint8_t port, uint32_t source, uint64_t inode, uint16_t id, uint16_t frag) {
	struct message *m, *tm;
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

		if (match == true) {
			tm = m->next;

			if (m->prev) m->prev->next = m->next;
			if (m->next) m->next->prev = m->prev;

			mutex_spin(&m_event_handler);

			if (event_handler[port]) {
				event_handler[port](m->source, m->packet);
			}
			
			mutex_free(&m_event_handler);

			free(m);
			m = tm;
		}
		else {
			m = m->next;
		}
	}

	mutex_free(&m_msg_queue[port]);
}

/****************************************************************************
 * dump
 */

void dump(uint8_t port) {
	_dumpm(port, 0, 0, 0, -1);
}

/****************************************************************************
 * dumps
 */

void dumps(uint8_t port, uint32_t source) {
	_dumpm(port, source, 0, 0, -1);
}

/****************************************************************************
 * dumpn
 */

void dumpn(uint8_t port, uint32_t source, uint64_t inode) {
	_dumpm(port, source, inode, 0, -1);
}

/****************************************************************************
 * dumpi
 */

void dumpi(uint8_t port, uint32_t source, uint16_t id) {
	_dumpm(port, source, 0, id, -1);
}

/****************************************************************************
 * dumpf
 */

void dumpf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag) {
	_dumpm(port, source, 0, id, frag);
}
