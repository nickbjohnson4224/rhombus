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
 *
 * protocol:
 *   port: PORT_CONS
 *
 *   request:
 *     uint32_t type
 *     char name[]
 *
 *   reply:
 *     uint64_t rp
 */

void __cons_wrapper(struct msg *msg) {
	struct vfs_obj *dir, *new_fobj;
	int type;
	uint64_t rp;
	const char *name;

	/* check request */
	if (msg->length < sizeof(uint32_t)) {
		merror(msg);
		return;
	}

	/* make sure the active driver can construct new objects */
	if (!_vfs_cons) {
		merror(msg);
		return;
	}

	/* extract data */
	type = ((uint32_t*) msg->data)[0];
	name = (const char*) &msg->data[4];

	/* get the requested parent directory */
	dir = vfs_get_index(RP_INDEX(msg->target));

	if (dir) {
		mutex_spin(&dir->mutex);

		/* check permissions */
		if ((acl_get(dir->acl, gettuser()) & PERM_WRITE) == 0) {
			rp = 0;
		}
		else {

			/* construct new object */
			new_fobj = _vfs_cons(type);

			if (new_fobj) {
				/* add new object to parent directory */
				vfs_dir_push(dir, new_fobj, name);

				/* return pointer to new object on success */
				rp = RP_CONS(getpid(), new_fobj->index);
			}
			else {
				rp = 0;
			}
		}

		mutex_free(&dir->mutex);
	}
	else {
		rp = 0;
	}

	msg->length = sizeof(uint64_t);
	((uint64_t*) msg->data)[0] = rp;
	mreply(msg);
}
