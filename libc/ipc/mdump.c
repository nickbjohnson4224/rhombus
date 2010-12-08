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
 * _mdumpm
 *
 * For all matching messages in the message queue, either resend their events
 * if possible or delete them. Used internally by dump* family of functions.
 */

static void _mdumpm(uint8_t port, uint32_t source) {
	struct msg *msg, *msg_temp;

	mutex_spin(&m_msg_queue[port]);
	msg = msg_queue[port].next;
	
	while (msg) {
		if (!source || source != msg->source) {
			msg_temp = msg->next;

			if (msg->prev) msg->prev->next = msg->next;
			if (msg->next) msg->next->prev = msg->prev;

			mutex_spin(&m_event_handler);

			if (event_handler[port]) {
				event_handler[port](msg);
			}
			
			mutex_free(&m_event_handler);

			free(msg);
			msg = msg_temp;
		}
		else {
			msg = msg->next;
		}
	}

	mutex_free(&m_msg_queue[port]);
}

/****************************************************************************
 * mdump
 */

void mdump(uint8_t port) {
	_mdumpm(port, 0);
}

/****************************************************************************
 * mdumps
 */

void mdumps(uint8_t port, uint32_t source) {
	_mdumpm(port, source);
}
