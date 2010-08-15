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
#include <mutex.h>
#include <ipc.h>

/****************************************************************************
 * _pdumpm
 *
 * For all matching messages in the message queue, either resend their events
 * if possible or delete them. Used internally by dump* family of functions.
 */

static void _pdumpm
	(uint8_t port, uint32_t source, uint32_t inode, uint16_t id, uint16_t frag) {
	struct message *m, *tm;
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
 * pdump
 */

void pdump(uint8_t port) {
	_pdumpm(port, 0, 0, 0, -1);
}

/****************************************************************************
 * pdumps
 */

void pdumps(uint8_t port, uint32_t source) {
	_pdumpm(port, source, 0, 0, -1);
}

/****************************************************************************
 * pdumpn
 */

void pdumpn(uint8_t port, uint32_t source, uint32_t inode) {
	_pdumpm(port, source, inode, 0, -1);
}

/****************************************************************************
 * pdumpi
 */

void pdumpi(uint8_t port, uint32_t source, uint16_t id) {
	_pdumpm(port, source, 0, id, -1);
}

/****************************************************************************
 * pdumpf
 */

void pdumpf(uint8_t port, uint32_t source, uint16_t id, uint16_t frag) {
	_pdumpm(port, source, 0, id, frag);
}
