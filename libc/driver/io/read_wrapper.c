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
 * __read_wrapper
 *
 * Handles and redirects read requests to the current active driver.
 */

void __read_wrapper(struct msg *msg) {
	struct resource *file;
	struct msg *reply;
	uint64_t offset;
	uint64_t source;
	uint32_t index;
	uint32_t size;

	if (msg->length != sizeof(uint64_t) + sizeof(uint32_t)) {
		merror(msg);
		return;
	}

	if (!_di_read) {
		merror(msg);
		return;
	}

	index = RP_INDEX(msg->target);

	file = index_get(index);
	if (!file) {
		merror(msg);
		return;
	}

	mutex_spin(&file->mutex);

	if (!vfs_permit(file, msg->source, PERM_READ)) {
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	if (!(file->type & FS_TYPE_FILE)) {
		mutex_free(&file->mutex);
		merror(msg);
		return;
	}

	mutex_free(&file->mutex);

	offset = ((uint64_t*) msg->data)[0];
	size   = ((uint32_t*) msg->data)[2];
	
	reply = aalloc(sizeof(struct msg) + size, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = size;
	reply->port   = PORT_REPLY;
	reply->arch   = ARCH_NAT;

	source = msg->source;
	free(msg);

	reply->length = _di_read(source, index, reply->data, size, offset);

	msend(reply);
}
