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

#include <rdi/core.h>
#include <rdi/io.h>
#include <rdi/access.h>

#include <stdlib.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <page.h>
#include <ipc.h>

/*****************************************************************************
 * _rdi_callback_share
 *
 * Share callback currently registered by the driver. Protected by the mutex
 * <_m_callback_share>. Do not modify directly.
 */

static int (*_rdi_callback_share)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
static bool _m_callback_share;

/*****************************************************************************
 * rdi_set_share
 *
 * Set the share callback in a thread-safe manner.
 */

void rdi_set_share(int (*_share)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off)) {
	
	mutex_spin(&_m_callback_share);
	_rdi_callback_share = _share;
	mutex_free(&_m_callback_share);
}

/*****************************************************************************
 * __rdi_share_handler
 *
 * Handles and redirects share requests to the share callback. See 
 * libc/natio/share.c for protocol details.
 */

void __rdi_share_handler(struct msg *msg) {
	struct resource *file;
	uint64_t offset;
	uint8_t err;
	void *pages;

	/* check request */
	if (msg->length < PAGESZ - sizeof(struct msg)) {
		merror(msg);
		return;
	}

	mutex_spin(&_m_callback_share);

	if (!_rdi_callback_share) {
		/* no share callback */
		mutex_free(&_m_callback_share);
		merror(msg);
		return;
	}

	file = index_get(RP_INDEX(msg->target));
	if (!file) {
		/* file not found */
		mutex_free(&_m_callback_share);
		merror(msg);
		return;
	}

	mutex_spin(&file->mutex);

	if (!vfs_permit(file, msg->source, PERM_WRITE)) {
		/* access denied */
		mutex_free(&_m_callback_share);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	/* extract data */
	offset = ((uint64_t*) msg->data)[0];
	pages = aalloc(msg->length - PAGESZ + sizeof(struct msg), PAGESZ);
	page_self(&msg->data[PAGESZ - sizeof(struct msg)], pages, msg->length - PAGESZ + sizeof(struct msg));

	/* call share callback */
	err = _rdi_callback_share(msg->source, RP_INDEX(msg->target),
		pages, msg->length - PAGESZ + sizeof(struct msg), offset);

	mutex_free(&_m_callback_share);
	mutex_free(&file->mutex);
	
	/* reply with error code */
	msg->data[0] = err;
	msg->length = 1;
	mreply(msg);
}
