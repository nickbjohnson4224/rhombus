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

#include <stdint.h>
#include <stdlib.h>
#include <mutex.h>
#include <abi.h>
#include <ipc.h>

/****************************************************************************
 * _precvm
 *
 * Attempts to find a matching message. Returns the found message on 
 * success, and NULL on failure or empty packet. Used internally by recv* 
 * family of functions.
 */

static struct packet *_precvm
	(uint8_t port, uint32_t source, uint32_t inode, uint16_t id, uint16_t frag) {
	struct message *m;
	struct packet *packet;
	bool match = false;

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
		free(m);
	}
	else {
		packet = NULL;
	}

	mutex_free(&m_msg_queue[port]);

	return packet;
}

/****************************************************************************
 * precv
 */

struct packet *precv(uint8_t port) {
	return _precvm(port, 0, 0, 0, -1);
}

/****************************************************************************
 * precvs
 */

struct packet *precvs(uint8_t port, uint32_t source) {
	return _precvm(port, source, 0, 0, -1);
}

/****************************************************************************
 * precvn
 */

struct packet *precvn(uint8_t port, uint32_t source, uint32_t inode) {
	return _precvm(port, source, inode, 0, -1);
}

/****************************************************************************
 * precvi
 */

struct packet *precvi(uint8_t port, uint32_t source, uint16_t id) {
	return _precvm(port, source, 0, id, -1);
}

/****************************************************************************
 * precvf
 */

struct packet *precvf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag) {
	return _precvm(port, source, 0, id, frag);
}
