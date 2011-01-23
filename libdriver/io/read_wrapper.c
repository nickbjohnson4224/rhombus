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
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

#include <driver/vfs.h>
#include <driver/io.h>

/*****************************************************************************
 * __read_wrapper
 *
 * Handles and redirects read requests to the current active driver.
 */

void __read_wrapper(struct msg *msg) {
	struct vfs_obj *file;
	struct mp_io *cmd;

	cmd = io_recv(msg);

	if (!cmd) {
		error_reply(msg, 1);
		return;
	}

	if (!_di_read) {
		error_reply(msg, 1);
		return;
	}

	file = vfs_get_index(cmd->index);

	if (!file || (file->type != FOBJ_FILE)) {
		error_reply(msg, 1);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & FS_PERM_READ)) {
		error_reply(msg, 1);
		return;
	}
	
	cmd->size = _di_read(file, cmd->data, cmd->size, cmd->offset);

	msend(PORT_REPLY, msg->source, msg);
}
