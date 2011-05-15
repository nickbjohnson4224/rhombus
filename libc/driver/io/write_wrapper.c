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
#include <mutex.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>
#include <vfs.h>

/*****************************************************************************
 * __write_wrapper
 *
 * Handles and redirects write requests to the current active driver.
 *
 * protocol:
 *   port: PORT_WRITE
 *
 *   request:
 *     uint64_t offset
 *     uint8_t data[]
 *
 *   reply:
 *     uint32_t size
 */

void __write_wrapper(struct msg *msg) {
	struct vfs_obj *file;
	uint64_t offset;

	if (msg->length < sizeof(uint64_t)) {
		merror(msg);
		return;
	}

	if (!_di_write) {
		merror(msg);
		return;
	}

	file = vfs_get(RP_INDEX(msg->target));
	if (!file || !(acl_get(file->acl, getuser(RP_PID(msg->source))) & PERM_WRITE)) {
		merror(msg);
		return;
	}

	if (!(file->type & FS_TYPE_FILE)) {
		merror(msg);
		return;
	}

	offset = ((uint64_t*) msg->data)[0];

	((uint32_t*) msg->data)[0] = _di_write(msg->source, RP_INDEX(msg->target), 
		&msg->data[8], msg->length - 8, offset);
	msg->length = sizeof(uint32_t);

	mreply(msg);
}
