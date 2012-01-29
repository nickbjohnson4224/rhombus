/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <string.h>

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/page.h>
#include <rho/ipc.h>

/*****************************************************************************
 * share (DEPRECATED)
 *
 * Share pages of memory with another process. The pages are shared with the
 * page permissions <prot>. This call also has an <offset>, which may be used
 * at the driver's discretion to make share act like read or write. Returns
 * zero on success, nonzero on error.
 *
 * protocol:
 *   action: ACTION_SHARE
 *
 *   request:
 *     uint64_t offset
 *     ... (to align to PAGESZ)
 *     uint8_t pages[]
 *
 *   reply:
 *     uint8_t err;
 */

int rp_share(uint64_t rp, void *buf, size_t size, uint64_t offset, int prot) {
	struct msg *msg;
	int err;

	/* check alignment */
	if ((uintptr_t) buf % PAGESZ) {
		return 1;
	}

	msg = aalloc(PAGESZ + size, PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CURRENT_THREAD;
	msg->target = rp;
	msg->length = PAGESZ - sizeof(struct msg) + size;
	msg->action = ACTION_SHARE;
	msg->arch   = ARCH_NAT;
	((uint64_t*) msg->data)[0] = offset;

	page_self(buf, &msg->data[PAGESZ - sizeof(struct msg)], size);
	page_prot(&msg->data[PAGESZ - sizeof(struct msg)], size, prot);

	if (msend(msg)) return 1;
	msg = mwait(ACTION_REPLY, rp);
	
	if (msg->length != 1) {
		err = 1;
	}
	else {
		err = msg->data[0];
	}

	free(msg);
	return err;
}
