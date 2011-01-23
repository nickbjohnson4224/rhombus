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
#include <proc.h>

/*****************************************************************************
 * __move_wrapper
 *
 * Performs the requested actions of a FS_MOVE command.
 */

void __move_wrapper(struct mp_fs *cmd) {
	struct vfs_obj *dir, *obj;	

	/* make sure the request is within the driver */
	if (RP_PID(cmd->v0) != getpid()) {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
		return;
	}

	/* get the requested object and new parent directory */
	dir = vfs_get_index(cmd->index);
	obj = vfs_get_index(RP_INDEX(cmd->v0));

	if (!dir || !obj) {
		/* return ERR_FILE on failure to find object or directory */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
		return;
	}

	mutex_spin(&dir->mutex);
	mutex_spin(&obj->mutex);

	/* check all permissions */
	if (((acl_get(dir->acl, gettuser()) & FS_PERM_WRITE) == 0) || 
		((acl_get(obj->mother->acl, gettuser()) & FS_PERM_WRITE) == 0)) {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_DENY;

		mutex_free(&dir->mutex);
		mutex_free(&obj->mutex);
		return;
	}

	mutex_free(&dir->mutex);

	/* remove object from its directory */
	vfs_dir_pull(obj);

	/* add object to new directory with name <cmd->s0> */
	vfs_dir_push(dir, obj, cmd->s0);

	mutex_free(&obj->mutex);

	/* return pointer to moved object on success */
	cmd->v0   = getpid();
	cmd->v0 <<= 32;
	cmd->v0  |= obj->index;
}
