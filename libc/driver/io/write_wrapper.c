/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * write_wrapper
 *
 * Handles and redirects write requests to the current active driver.
 */

void write_wrapper(struct msg *msg) {
	struct fs_obj *file;
	struct io_cmd *cmd;

	if (!msg->packet) {
		msend(PORT_REPLY, msg->source, msg);
		return;
	}

	cmd = msg->packet;

	if (!active_driver->write) {
		cmd->length = 0;
		msend(PORT_REPLY, msg->source, msg);
		return;
	}

	file = lfs_lookup(msg->value);

	if (!file || (file->type != FOBJ_FILE)) {
		cmd->length = 0;
		msend(PORT_REPLY, msg->source, msg);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & ACL_WRITE)) {
		cmd->length = 0;
		msend(PORT_REPLY, msg->source, msg);
		return;
	}
	
	cmd->length = active_driver->write(file, cmd->data, cmd->length, cmd->offset);

	msend(PORT_REPLY, msg->source, msg);
}
