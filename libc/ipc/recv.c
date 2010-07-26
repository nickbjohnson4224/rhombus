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
 * _recvm
 *
 * Attempts to find a matching message. Returns the found message on 
 * success, and NULL on failure or empty packet. Used internally by recv* 
 * family of functions.
 */

static struct packet *_recvm
	(uint8_t port, uint32_t source, uint64_t inode, uint16_t id, uint16_t frag) {
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
