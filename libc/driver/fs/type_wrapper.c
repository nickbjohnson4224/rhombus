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

/*****************************************************************************
 * type_wrapper
 *
 * Performs the requested actions of a FS_TYPE command.
 */

void type_wrapper(struct fs_cmd *cmd, uint32_t inode) {
	struct fs_obj *fobj;
	
	/* get the requested object */
	fobj = lfs_lookup(inode);

	if (fobj) {
		mutex_spin(&fobj->mutex);
		
		/* check permissions */
		if ((acl_get(fobj->acl, gettuser()) & ACL_READ) == 0) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_DENY;
		}
		else {
			/* return the type of the object */
			cmd->v0 = fobj->type;
		}

		mutex_free(&fobj->mutex);
	}
	else {
		/* return ERR_FILE on failure to find object */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
