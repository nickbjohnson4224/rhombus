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
 * _rdi_callback_sync
 *
 * Sync callback currently registered by the driver. Protected by the mutex
 * <_m_callback_sync>. Do not modify directly.
 */

static void (*_rdi_callback_sync)(uint64_t src, uint32_t idx);
static bool _m_callback_sync;

/*****************************************************************************
 * rdi_set_sync
 *
 * Set the sync callback in a thread-safe manner.
 */

void rdi_set_sync(void (*_sync)(uint64_t src, uint32_t idx)) {
	
	mutex_spin(&_m_callback_sync);
	_rdi_callback_sync = _sync;
	mutex_free(&_m_callback_sync);
}

/*****************************************************************************
 * __rdi_sync_handler
 *
 * Handles and redirects sync requests to the sync callback. See natio/sync.c 
 * for protocol details.
 */

void __rdi_sync_handler(struct msg *msg) {
	struct resource *file;

	mutex_spin(&_m_callback_sync);

	if (!_rdi_callback_sync) {
		/* no sync callback */
		mutex_free(&_m_callback_sync);
		merror(msg);
		return;
	}
	
	file = index_get(RP_INDEX(msg->target));
	if (!file) {
		/* file not found */
		mutex_free(&_m_callback_sync);
		merror(msg);
		return;
	}

	mutex_spin(&file->mutex);

	if (!vfs_permit(file, msg->source, PERM_WRITE)) {
		/* access denied */
		mutex_free(&_m_callback_sync);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	if (!FS_IS_FILE(file->type)) {
		/* resource is not a file */
		mutex_free(&_m_callback_sync);
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	/* call sync callback */
	_rdi_callback_sync(msg->source, RP_INDEX(msg->target));

	mutex_free(&_m_callback_sync);
	mutex_free(&file->mutex);

	/* note: errors are the same as valid replies */
	merror(msg);
}
