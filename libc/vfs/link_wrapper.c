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
 * __link_wrapper
 *
 * Performs the requested actions of a FS_LINK command.
 *
 * protocol:
 *   port: PORT_LINK
 *
 *   request:
 *     uint64_t rp
 *
 *   reply:
 *     uint8_t err
 */

void __link_wrapper(struct msg *msg) {
	struct vfs_obj *dir;
	uint64_t rp;
	uint8_t err = 0;

	/* check request */
	if (msg->length != sizeof(uint64_t)) {
		merror(msg);
		return;
	}

	/* extract data */
	rp = ((uint64_t*) msg->data)[0];

	/* get the requested directory */
	dir = vfs_get(RP_INDEX(msg->target));

	if (dir) {
		mutex_spin(&dir->mutex);

		/* check type of object */
		if ((dir->type & TYPE_DIR) == 0) {
			err = 1;
		}

		/* check all permissions */
		else if ((acl_get(dir->acl, gettuser() & PERM_WRITE) == 0)) {
			err = 1;
		}

		else {
			/* set link location */
			dir->link = rp;
		}

		mutex_free(&dir->mutex);
	}
	else {
		/* return ERR_FILE on failure to find directory */
		err = 1;
	}

	if (err) {
		merror(msg);
	}
	else {
		msg->length = 1;
		msg->data[0] = 0;
		mreply(msg);
	}
}
