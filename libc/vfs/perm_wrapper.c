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
#include <natio.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __perm_wrapper
 *
 * Performs the requested actions of a FS_PERM command.
 *
 * protocol:
 *   port: PORT_PERM
 *
 *   request:
 *     uint32_t user
 *
 *   reply:
 *     uint8_t perm
 */

void __perm_wrapper(struct msg *msg) {
	struct vfs_obj *fobj;
	uint32_t user;
	uint8_t perm;

	/* check request */
	if (msg->length != sizeof(uint32_t)) {
		merror(msg);
		return;
	}

	/* extract data */
	user = ((uint32_t*) msg->data)[0];
	
	/* look up the requested object */
	fobj = vfs_get(RP_INDEX(msg->target));

	if (fobj) {
		mutex_spin(&fobj->mutex);

		/* return the permissions of the object for user <cmd->v0> */	
		perm = acl_get(fobj->acl, user);

		mutex_free(&fobj->mutex);
	}
	else {
		perm = 0;
	}

	msg->length = 1;
	msg->data[0] = perm;
	mreply(msg);
}
