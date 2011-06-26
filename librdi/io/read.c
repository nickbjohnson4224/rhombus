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
#include <ipc.h>

/*****************************************************************************
 * _rdi_callback_read
 *
 * Read callback currently registered by the driver. Protected by the mutex
 * <_m_callback_read>. Do not modify directly.
 */

static size_t (*_rdi_callback_read)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
static bool _m_callback_read;

/*****************************************************************************
 * rdi_set_read
 *
 * Set the read callback in a thread-safe manner.
 */

void rdi_set_read(size_t (*_read)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off)) {
	
	mutex_spin(&_m_callback_read);
	_rdi_callback_read = _read;
	mutex_free(&_m_callback_read);

	/* also make sure the read handler is registered */
	when(PORT_READ, __rdi_read_handler);
}

/*****************************************************************************
 * __rdi_read_handler
 *
 * Handles and redirects read requests to the read callback. See natio/read.c
 * for protocol details.
 */

void __rdi_read_handler(struct msg *msg) {
	struct resource *file;
	struct msg *reply;
	uint64_t offset;
	uint64_t source;
	uint32_t index;
	uint32_t size;

	if (msg->length != sizeof(uint64_t) + sizeof(uint32_t)) {
		/* message is of the wrong size */
		merror(msg);
		return;
	}

	mutex_spin(&_m_callback_read);
	if (!_rdi_callback_read) {
		/* there is no read callback */
		mutex_free(&_m_callback_read);
		merror(msg);
		return;
	}

	index = RP_INDEX(msg->target);

	file = index_get(index);
	if (!file) {
		/* there is no corresponding resource in the index */
		mutex_free(&_m_callback_read);
		merror(msg);
		return;
	}

	mutex_spin(&file->mutex);

	if (!vfs_permit(file, msg->source, PERM_READ)) {
		/* access denied */
		mutex_free(&_m_callback_read);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	if (!(file->type & FS_TYPE_FILE)) {
		/* resource is not a file */
		mutex_free(&_m_callback_read);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	/* read parameters */
	offset = ((uint64_t*) msg->data)[0];
	size   = ((uint32_t*) msg->data)[2];
	
	/* construct reply message containing buffer */
	reply = aalloc(sizeof(struct msg) + size, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = size;
	reply->port   = PORT_REPLY;
	reply->arch   = ARCH_NAT;

	source = msg->source;
	free(msg);

	/* call read callback */
	reply->length = _rdi_callback_read(source, index, reply->data, size, offset);

	mutex_free(&_m_callback_read);
	mutex_free(&file->mutex);

	msend(reply);
}
