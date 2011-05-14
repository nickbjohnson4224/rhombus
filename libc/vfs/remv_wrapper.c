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

#include <stdlib.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __remv_wrapper
 *
 * Performs the requested actions of a FS_REMV command.
 *
 * protocol:
 *   port: PORT_REMV
 *
 *   request:
 *   
 *   reply:
 *     uint8_t err;
 */

void __remv_wrapper(struct msg *msg) {
	struct vfs_obj *fobj;
	
	/* get the requested object */
	fobj = vfs_get(RP_INDEX(msg->target));

	if (fobj) {
		mutex_spin(&fobj->mutex);

		/* check all permissions */
		if ((acl_get(fobj->acl, gettuser()) & PERM_WRITE) == 0 ||
			(acl_get(fobj->mother->acl, gettuser() & PERM_WRITE) == 0)) {
			merror(msg);
			return;
		}

		/* check if directory is empty */
		if (fobj->type & TYPE_DIR && fobj->daughter) {
			merror(msg);
			return;
		}

		/* remove the object from its directory */
		vfs_pull(msg->source, fobj);

		if (_vfs_free) {
			/* allow the driver to free the object */
			if (_vfs_free(msg->source, fobj)) {
				mutex_free(&fobj->mutex);
				merror(msg);
				return;
			}
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
		merror(msg);
	}

	msg->length = 1;
	msg->data[0] = 0;
	mreply(msg);
}
