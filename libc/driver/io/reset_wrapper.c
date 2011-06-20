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

#include <driver.h>
#include <stdlib.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>
#include <vfs.h>

/*****************************************************************************
 * __reset_wrapper
 *
 * Handles and redirects reset requests to the current active driver.
 */

void __reset_wrapper(struct msg *msg) {
	struct resource *file;

	if (!_di_reset) {
		merror(msg);
		return;
	}
	
	file = index_get(RP_INDEX(msg->target));
	if (!file) {
		merror(msg);
		return;
	}

	mutex_spin(&file->mutex);

	if (!vfs_permit(file, msg->source, PERM_WRITE)) {
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	mutex_free(&file->mutex);

	_di_reset(msg->source, RP_INDEX(msg->target));

	merror(msg); // errors are the same as valid replies
}
