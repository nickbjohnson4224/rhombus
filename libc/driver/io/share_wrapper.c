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

#include <driver.h>
#include <stdlib.h>
#include <mutex.h>
#include <natio.h>
#include <proc.h>
#include <page.h>
#include <ipc.h>
#include <vfs.h>

void __share_wrapper(struct msg *msg) {
	struct vfs_obj *file;
	uint64_t offset;
	uint8_t err;
	void *pages;

	/* check request */
	if (msg->length < PAGESZ - sizeof(struct msg)) {
		merror(msg);
		return;
	}

	if (!_di_share) {
		merror(msg);
		return;
	}

	file = vfs_get(RP_INDEX(msg->target));

	if (!file) {
		merror(msg);
		return;
	}

	if (!(acl_get(file->acl, RP_PID(msg->source)) & PERM_WRITE)) {
		merror(msg);
		return;
	}

	/* extract data */
	offset = ((uint64_t*) msg->data)[0];
	pages = aalloc(msg->length - PAGESZ + sizeof(struct msg), PAGESZ);
	page_self(&msg->data[PAGESZ - sizeof(struct msg)], pages, msg->length - PAGESZ + sizeof(struct msg));

	err = _di_share(msg->source, RP_INDEX(msg->target), 
		pages, msg->length - PAGESZ + sizeof(struct msg), offset);

	msg->data[0] = err;
	msg->length = 1;
	mreply(msg);
}
