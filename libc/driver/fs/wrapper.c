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

typedef void (*lfs_wrapper_t)(struct fs_cmd *cmd, uint32_t inode);

static lfs_wrapper_t lfs_wrapper_v[12] = {
	NULL,
	find_wrapper,
	cons_wrapper,
	move_wrapper,
	remv_wrapper,
	link_wrapper,
	list_wrapper,
	size_wrapper,
	type_wrapper,
	lfnd_wrapper,
	perm_wrapper,
	auth_wrapper
};	

/*****************************************************************************
 * lfs_wrapper
 *
 * Handles all filesystem requests.
 */

void lfs_wrapper(struct msg *msg) {
	struct io_cmd *io_cmd;
	struct fs_cmd *cmd;

	/* reject null packets */
	if (!msg->packet) {
		msend(PORT_REPLY, msg->source, msg);
		return;
	}

	io_cmd = msg->packet;

	/* reject invalid packets */
	if (io_cmd->length != sizeof(struct fs_cmd)) {
		io_cmd->length = 0;
		msend(PORT_REPLY, msg->source, msg);
		return;
	}

	cmd = (void*) io_cmd->data;
	cmd->null0 = '\0';

	/* perform action */
	if ((cmd->op < 12) && lfs_wrapper_v[cmd->op]) {
		lfs_wrapper_v[cmd->op](cmd, msg->value);
	}
	else {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FUNC;
	}

	msend(PORT_REPLY, msg->source, msg);
}

/*****************************************************************************
 * read_wrapper
 *
 * Handles and redirects read requests to the current active driver.
 */

void read_wrapper(struct msg *msg) {
	struct fs_obj *file;
	struct io_cmd *cmd;
	
	if (!msg->packet) {
		msend(PORT_REPLY, msg->source, msg);
		return;
	}

	cmd = msg->packet;

	if (!active_driver->read) {
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

	if (!(acl_get(file->acl, gettuser()) & ACL_READ)) {
		cmd->length = 0;
		msend(PORT_REPLY, msg->source, msg);
		return;
	}
	
	cmd->length = active_driver->read(file, cmd->data, cmd->length, cmd->offset);

	msend(PORT_REPLY, msg->source, msg);
}

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

/*****************************************************************************
 * sync_wrapper
 *
 * Handles and redirects sync requests to the current active driver.
 */

void sync_wrapper(struct msg *msg) {
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

	active_driver->sync(file);

	msend(PORT_REPLY, msg->source, msg);
}

/*****************************************************************************
 * reset_wrapper
 *
 * Handles and redirects reset requests to the current active driver.
 */

void reset_wrapper(struct msg *msg) {
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

	active_driver->reset(file);

	msend(PORT_REPLY, msg->source, msg);
}
