/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

/*****************************************************************************
 * __link_wrapper
 *
 * Performs the requested actions of a FS_LINK command.
 */

void __link_wrapper(struct mp_fs *cmd) {
	struct vfs_obj *dir;
	
	/* get the requested directory */
	dir = vfs_get_index(cmd->index);

	if (dir) {
		mutex_spin(&dir->mutex);

		/* check type of object */
		if (dir->type != FOBJ_DIR) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_TYPE;
			return;
		}

		/* check all permissions */
		if ((acl_get(dir->acl, gettuser() & FS_PERM_WRITE) == 0)) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_DENY;
			return;
		}
	
		/* set link location */
		dir->link = cmd->v0;

		mutex_free(&dir->mutex);
	}
	else {
		/* return ERR_FILE on failure to find directory */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
