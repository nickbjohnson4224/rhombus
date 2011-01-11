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
 * remv_wrapper
 *
 * Performs the requested actions of a FS_REMV command.
 */

void remv_wrapper(struct mp_fs *cmd) {
	struct fs_obj *fobj;
	
	/* get the requested object */
	fobj = lfs_lookup(cmd->index);

	if (fobj) {
		mutex_spin(&fobj->mutex);

		/* check all permissions */
		if ((acl_get(fobj->acl, gettuser()) & FS_PERM_WRITE) == 0 ||
			(acl_get(fobj->mother->acl, gettuser() & FS_PERM_WRITE) == 0)) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_DENY;
			return;
		}

		/* check if directory is empty */
		if (fobj->type == FOBJ_DIR && fobj->daughter) {
			cmd->op = FS_ERR;
			cmd->v0 = ERR_FULL;
			return;
		}

		/* remove the object from its directory */
		lfs_pull(fobj);

		if (active_driver->free) {
			/* allow the driver to free the object */
			active_driver->free(fobj);
		}
		else {
			/* free the object, assuming data is not allocated */
			acl_free(fobj->acl);
			free(fobj);
		}

		mutex_free(&fobj->mutex);
	}
	else {
		/* return ERR_FILE on failure to find object */
		cmd->op = FS_ERR;
		cmd->v0 = ERR_FILE;
	}
}
