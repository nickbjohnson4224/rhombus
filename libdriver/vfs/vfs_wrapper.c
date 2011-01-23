/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <stdio.h>
#include <proc.h>
#include <ipc.h>

typedef void (*vfs_wrapper_t)(struct mp_fs *cmd);

static vfs_wrapper_t vfs_wrapper_v[256] = {
	NULL,
	__find_wrapper,
	__cons_wrapper,
	__move_wrapper,
	__remv_wrapper,
	__link_wrapper,
	__list_wrapper,
	__size_wrapper,
	__type_wrapper,
	__lfnd_wrapper,
	__perm_wrapper,
	__auth_wrapper
};	

/*****************************************************************************
 * __vfs_wrapper
 *
 * Handles all filesystem requests.
 */

void __vfs_wrapper(struct msg *msg) {
	struct mp_fs *cmd;

	/* reject null packets */
	if (!msg->packet) {
		error_reply(msg, 1);
		return;
	}

	cmd = msg->packet;

	/* reject invalid packets */
	if (cmd->length != sizeof(struct mp_fs)) {
		error_reply(msg, 1);
		return;
	}

	cmd->null0 = '\0';

	/* perform action */
	if (vfs_wrapper_v[cmd->op]) {
		vfs_wrapper_v[cmd->op](cmd);
	}
	else {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FUNC;
	}

	msend(PORT_REPLY, msg->source, msg);
}

/*****************************************************************************
 * vfs_wrap_cmd
 *
 * Set the action to be performed on VFS opcode <op> to <vfs_cmd>. Returns
 * zero on success, nonzero on error.
 */

int vfs_wrap_cmd(uint8_t op, void (*vfs_cmd)(struct mp_fs *cmd)) {
	vfs_wrapper_v[op] = vfs_cmd;

	return 0;
}

/*****************************************************************************
 * vfs_wrap_init
 *
 * Initialize VFS request handling system. Returns zero on success, nonzero on
 * error.
 */

int vfs_wrap_init(void) {
	when(PORT_FS, __vfs_wrapper);

	return 0;
}
