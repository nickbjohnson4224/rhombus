/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <stdbool.h>
#include <mutex.h>
#include <ipc.h>

/****************************************************************************
 * _waitm
 */

static struct packet *_waitm
	(uint8_t port, uint32_t source, uint64_t inode, uint16_t id, uint16_t frag) {
	struct message *m;
	struct packet *packet;
	bool match;

	do {
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

		if (m) {
			if (m->next) m->next->prev = m->prev;
			if (m->prev) m->prev->next = m->next;

			packet = m->packet;
			free(m);
			break;
		}

		mutex_free(&m_msg_queue[port]);

	} while (1);

	return packet;
}

/****************************************************************************
 * wait
 */

struct packet *wait(uint8_t port) {
	return _waitm(port, 0, 0, 0, -1);
}

/****************************************************************************
 * waits
 */

struct packet *waits(uint8_t port, uint32_t source) {
	return _waitm(port, source, 0, 0, -1);
}

/****************************************************************************
 * waitn
 */

struct packet *waitn(uint8_t port, uint32_t source, uint64_t inode) {
	return _waitm(port, source, inode, 0, -1);
}

/****************************************************************************
 * waiti
 */

struct packet *waiti(uint8_t port, uint32_t source, uint16_t id) {
	return _waitm(port, source, 0, id, -1);
}

/****************************************************************************
 * waitf
 */

struct packet *waitf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag) {
	return _waitm(port, source, 0, id, frag);
}
