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
#include <stdbool.h>
#include <mutex.h>
#include <ipc.h>

/****************************************************************************
 * _waitm
 *
 * Waits until a matching message is in the message queue. Returns the found
 * message, which may be NULL if the packet is empty. Used internally by the
 * wait* family of functions.
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
