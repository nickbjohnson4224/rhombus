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
 * _rdi_callback_write
 *
 * Write callback currently registered by the driver. Protected by the mutex
 * <_m_callback_read>. Do not modify directly.
 */

static size_t (*_rdi_callback_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
static bool _m_callback_write;

/*****************************************************************************
 * rdi_set_write
 *
 * Set the write callback in a thread-safe manner.
 */

void rdi_set_write(size_t (*_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off)) {
	
	mutex_spin(&_m_callback_write);
	_rdi_callback_write = _write;
	mutex_free(&_m_callback_write);

	/* also make sure the read handler is registered */
	when(PORT_WRITE, __rdi_write_handler);
}

/*****************************************************************************
 * __rdi_write_handler
 *
 * Handles and redirects write requests to the read callback. See 
 * natio/write.c for protocol details.
 */

void __rdi_write_handler(struct msg *msg) {
	struct resource *file;
	uint64_t offset;

	if (msg->length < sizeof(uint64_t)) {
		/* message is the wrong length */
		merror(msg);
		return;
	}

	mutex_spin(&_m_callback_write);

	if (!_rdi_callback_write) {
		/* no write callback */
		mutex_free(&_m_callback_write);
		merror(msg);
		return;
	}

	file = index_get(RP_INDEX(msg->target));
	if (!file) {
		/* file not found */
		mutex_free(&_m_callback_write);
		merror(msg);
		return;
	}

	mutex_spin(&file->mutex);

	if (!vfs_permit(file, msg->source, PERM_WRITE)) {
		/* access denied */
		mutex_free(&_m_callback_write);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	if (!(file->type & FS_TYPE_FILE)) {
		/* resource is not a file */
		mutex_free(&_m_callback_write);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	/* extract parameter */
	offset = ((uint64_t*) msg->data)[0];

	/* call write callback */
	((uint32_t*) msg->data)[0] = _rdi_callback_write(msg->source, RP_INDEX(msg->target), 
		&msg->data[8], msg->length - 8, offset);
	msg->length = sizeof(uint32_t);

	mutex_free(&_m_callback_write);
	mutex_free(&file->mutex);

	mreply(msg);
}
