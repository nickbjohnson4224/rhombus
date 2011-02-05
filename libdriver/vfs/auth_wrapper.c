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
 * __auth_wrapper
 *
 * Performs the requested actions of a FS_AUTH command.
 *
 * protocol:
 *   port: PORT_AUTH
 *
 *   request:
 *     uint32_t user
 *     uint8_t perm
 *
 *   reply:
 *     uint8_t ret
 */

void __auth_wrapper(struct msg *msg) {
	struct vfs_obj *fobj;
	uint32_t user;
	uint8_t perm;
	int err = 0;
	
	/* check message */
	if (msg->length != sizeof(uint32_t) + sizeof(uint8_t)) {
		merror(msg);
	}

	/* get data */
	user = ((uint32_t*) msg->data)[0];
	perm = msg->data[4];

	/* get the requested object */
	fobj = vfs_get_index(RP_INDEX(msg->target));

	if (fobj) {
		mutex_spin(&fobj->mutex);

		/* check permissions */
		if ((acl_get(fobj->acl, gettuser()) & PERM_ALTER) == 0) {
			err = 1;
		}
		else {
			/* set the permissions on the object for user <cmd->v0> */
			acl_set(fobj->acl, user, perm);
		}

		mutex_free(&fobj->mutex);
	}
	else {
		/* return ERR_FILE on failure to find object */
		err = 1;
	}

	msg->length = sizeof(uint8_t);
	msg->data[0] = err;
	mreply(msg);
}
