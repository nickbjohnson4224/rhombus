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
 * __list_wrapper
 *
 * Performs the requested actions of a FS_LIST command.
 */

void __list_wrapper(struct mp_fs *cmd) {
	struct vfs_obj *dir;
	
	/* get requested directory */
	dir = vfs_get_index(cmd->index);

	if (dir) {
		mutex_spin(&dir->mutex);

		if (dir->type == FOBJ_DIR) {

			/* check permissions */
			if ((acl_get(dir->acl, gettuser()) & FS_PERM_READ) == 0) {
				cmd->op = FS_ERR;
				cmd->v0 = ERR_DENY; 
			}
			else if (vfs_dir_list(dir, cmd->v0, cmd->s0, 4000)) {
				/* return ERR_FILE if the entry does not exist */
				cmd->op = FS_ERR;
				cmd->v0 = ERR_FILE;
			}
		}
		else {
			/* return ERR_TYPE if <dir> is not a directory */
			cmd->op = FS_ERR;
			cmd->v0 = ERR_TYPE;
		}

		mutex_free(&dir->mutex);
	}
	else {
		/* return ERR_FILE on failure to find directory */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
