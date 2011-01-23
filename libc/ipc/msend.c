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

#include <stdlib.h>
#include <stdint.h>
#include <abi.h>
#include <ipc.h>

/****************************************************************************
 * msend
 *
 * Sends the message <msg> to the target <target> at port <port>. Returns 
 * zero on success, nonzero on error. The message <msg> is freed on success.
 */

int msend(uint8_t port, uint32_t target, struct msg *msg) {
	uint32_t err;

	if (!msg) {
		err = _send(0, 0, port, target);
	}
	else {
		err = _send((uintptr_t) msg->packet, msg->count, port, target);
	}

	if (err) {
		return 1;
	}
	else {

		if (msg) {
			if (msg->packet) {
				free(msg->packet);
			}
	
			free(msg);
		}

		return 0;
	}
}
