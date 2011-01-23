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

/*****************************************************************************
 * __size_wrapper
 *
 * Performs the requested actions of a FS_SIZE command.
 */

void __size_wrapper(struct mp_fs *cmd) {
	struct vfs_obj *file;
	
	/* get requested file */
	file = vfs_get_index(cmd->index);

	if (file) {
		mutex_spin(&file->mutex);

		/* check to make sure <file> is a file */
		if (file->type != FOBJ_FILE) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_TYPE;
		}

		/* check all permissions */
		else if ((acl_get(file->acl, gettuser()) & FS_PERM_READ) == 0) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_DENY;
		}

		else if (_di_size) {
			/* allow driver to figure out the file's size */
			cmd->v0 = _di_size(file);
		}
		else {
			/* default to <file->size> for size */
			cmd->v0 = file->size;
		}

		mutex_free(&file->mutex);
	}
	else {
		/* return ERR_FILE on failure to find file */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
