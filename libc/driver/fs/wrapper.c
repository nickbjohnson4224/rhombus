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

void lfs_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_cmd *cmd;

	/* reject null packets */
	if (!packet) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	/* reject invalid packets */
	if (packet->data_length != sizeof(struct fs_cmd)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	cmd = pgetbuf(packet);
	cmd->null0 = '\0';

	/* perform action */
	if ((cmd->op < 12) && lfs_wrapper_v[cmd->op]) {
		lfs_wrapper_v[cmd->op](cmd, packet->target_inode);
	}
	else {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FUNC;
	}

	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * read_wrapper
 *
 * Handles and redirects read requests to the current active driver.
 */

void read_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;
	
	if (!packet || !active_driver->read) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);

	if (!file || (file->type != FOBJ_FILE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & ACL_READ)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}
	
	packet->data_length = active_driver->read
		(file, pgetbuf(packet), packet->data_length, packet->offset);

	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * write_wrapper
 *
 * Handles and redirects write requests to the current active driver.
 */

void write_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;

	if (!packet || !active_driver->write) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);

	if (!file || (file->type != FOBJ_FILE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & ACL_WRITE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}
	
	packet->data_length = active_driver->write
		(file, pgetbuf(packet), packet->data_length, packet->offset);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * sync_wrapper
 *
 * Handles and redirects sync requests to the current active driver.
 */

void sync_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;

	if (!packet || !active_driver->sync) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);
	
	if (!file) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & ACL_WRITE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	active_driver->sync(file);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

/*****************************************************************************
 * reset_wrapper
 *
 * Handles and redirects reset requests to the current active driver.
 */

void reset_wrapper(struct packet *packet, uint8_t port, uint32_t caller) {
	struct fs_obj *file;

	if (!packet || !active_driver->reset) {
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	file = lfs_lookup(packet->target_inode);
	
	if (!file || (file->type != FOBJ_FILE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}

	if (!(acl_get(file->acl, gettuser()) & ACL_WRITE)) {
		pfree(packet);
		psend(PORT_REPLY, caller, NULL);
		return;
	}
	
	active_driver->reset(file);
	
	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}
