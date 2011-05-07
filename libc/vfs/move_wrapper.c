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

#include <string.h>
#include <stdlib.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __move_wrapper
 *
 * Performs the requested actions of a FS_MOVE command.
 *
 * protocol:
 *   port: PORT_MOVE
 *
 *   request:
 *     uint64_t fobj
 *     char name[]
 *
 *   reply:
 *     uint64_t fobj
 */

void __move_wrapper(struct msg *msg) {
	struct vfs_obj *dir, *obj;
	uint64_t fobj;
	const char *name;

	/* check message */
	if (msg->length < sizeof(uint64_t) + 1) {
		merror(msg);
		return;
	}

	/* extract data */
	fobj = ((uint64_t*) msg->data)[0];
	name = (const char*) &msg->data[8];

	/* make sure the request is within the driver */
	if (RP_PID(fobj) != getpid()) {
		merror(msg);
		return;
	}

	/* get the requested object and new parent directory */
	dir = vfs_get(RP_INDEX(msg->target));
	obj = vfs_get(RP_INDEX(fobj));

	if (!dir || !obj) {
		merror(msg);
		return;
	}

	mutex_spin(&dir->mutex);
	mutex_spin(&obj->mutex);

	/* check all permissions */
	if (((acl_get(dir->acl, gettuser()) & PERM_WRITE) == 0) || 
		((acl_get(obj->mother->acl, gettuser()) & PERM_WRITE) == 0)) {
		mutex_free(&dir->mutex);
		mutex_free(&obj->mutex);

		merror(msg);
		return;
	}

	mutex_free(&dir->mutex);

	/* remove object from its directory */
	if (vfs_pull(msg->source, obj)) {
		merror(msg);
		return;
	}

	/* add object to new directory with name <name> */
	if (vfs_push(msg->source, dir, obj)) {
		merror(msg);
		return;
	}
	else {
		obj->name = strdup(name);
	}

	mutex_free(&obj->mutex);

	/* return pointer to moved object on success */
	msg->length = sizeof(uint64_t);
	((uint64_t*) msg->data)[0] = RP_CONS(getpid(), obj->index);
	mreply(msg);
}
