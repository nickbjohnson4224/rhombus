/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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
 * _mrecvm
 *
 * Attempts to find a matching message. Returns the found message on 
 * success, and NULL on failure. Used internally by mrecv* family of 
 * functions.
 */

static struct msg *_mrecvm(uint8_t port, uint32_t source) {
	struct msg *msg;

	mutex_spin(&m_msg_queue[port]);
	msg = msg_queue[port].next;
	
	while (msg) {
		if (!source || source == msg->source) {
			break;
		}

		msg = msg->next;
	}

	if (msg) {
		if (msg->prev) msg->prev->next = msg->next;
		if (msg->next) msg->next->prev = msg->prev;
	}

	mutex_free(&m_msg_queue[port]);

	return msg;
}

/****************************************************************************
 * mrecv
 */

struct msg *mrecv(uint8_t port) {
	return _mrecvm(port, 0);
}

/****************************************************************************
 * mrecvs
 */

struct msg *mrecvs(uint8_t port, uint32_t source) {
	return _mrecvm(port, source);
}
