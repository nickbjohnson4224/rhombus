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
#include <stdio.h>
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * __cons_wrapper
 *
 * Performs the requested actions of a FS_CONS command.
 */

void __cons_wrapper(struct mp_fs *cmd) {
	struct vfs_obj *dir, *new_fobj;

	/* make sure the active driver can construct new objects */
	if (!_vfs_cons) {
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FUNC;
		return;
	}

	/* get the requested parent directory */
	dir = vfs_get_index(cmd->index);

	if (dir) {
		mutex_spin(&dir->mutex);

		/* check permissions */
		if ((acl_get(dir->acl, gettuser()) & FS_PERM_WRITE) == 0) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_DENY;
			return;
		}

		/* construct new object */
		new_fobj = _vfs_cons(cmd->v0);

		if (new_fobj) {
			/* add new object to parent directory */
			vfs_dir_push(dir, new_fobj, cmd->s0);

			/* return pointer to new object on success */
			cmd->v0   = getpid();
			cmd->v0 <<= 32;
			cmd->v0  |= new_fobj->index;
		}
		else {
			/* return ERR_NULL on failure to create file */
			cmd->op = FS_ERR;
			cmd->v0 = ERR_NULL;
		}

		mutex_free(&dir->mutex);
	}
	else {
		/* return ERR_FILE on failure to find directory */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
