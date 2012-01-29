/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/page.h>
#include <rho/abi.h>
#include <rho/ipc.h>

int msend(struct msg *msg) {
	uint8_t  action;
	uint32_t count;
	uint32_t target_pid;
	int err;

	/* check message and alignment */
	if (!msg || ((uintptr_t) msg % PAGESZ)) {
		return 1;
	}

	/* check source PID (do not send lies) */
	if (RP_PID(msg->source) != getpid()) {
		return 1;
	}

	/* extract data */
	action = msg->action;
	target_pid = RP_PID(msg->target);
	count = msg->length + sizeof(struct msg);

	/* calculate page count */
	count = (count % PAGESZ) ? (count / PAGESZ) + 1 : count / PAGESZ;

	err = _send((uintptr_t) msg, count, action, target_pid);
	free(msg);

	return err;
}
