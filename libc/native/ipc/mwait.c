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
 * _mwaitm
 *
 * Waits until a matching message is in the message queue. Returns the found
 * message, which may be NULL if the packet is empty. Used internally by the
 * wait* family of functions.
 */

static struct msg *_mwaitm(uint8_t port, uint32_t source) {
	struct msg *msg;
	event_t old_event;

	old_event = when(port, NULL);

	mutex_spin(&m_msg_queue[port]);

	do {
		msg = msg_queue[port].next;

		while (msg) {
			if (!source || source == msg->source) {
				break;
			}

			msg = msg->next;
		}

		if (msg) {
			if (msg->next) msg->next->prev = msg->prev;
			if (msg->prev) msg->prev->next = msg->next;
			break;
		}

	} while (1);

	mutex_free(&m_msg_queue[port]);

	when(port, old_event);

	return msg;
}

/****************************************************************************
 * mwait
 */

struct msg *mwait(uint8_t port) {
	return _mwaitm(port, 0);
}

/****************************************************************************
 * mwaits
 */

struct msg *mwaits(uint8_t port, uint32_t source) {
	return _mwaitm(port, source);
}
