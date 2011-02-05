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
#include <string.h>
#include <stdlib.h>
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * __list_wrapper
 *
 * Performs the requested actions of a FS_LIST command.
 *
 * protocol:
 *   port: PORT_LIST
 *
 *   request:
 *     uint32_t entry
 *
 *   reply:
 *     char name[]
 */

void __list_wrapper(struct msg *msg) {
	struct vfs_obj *dir;
	uint32_t entry;
	char buffer[100];
	int err = 0;
	
	/* check request */
	if (msg->length != sizeof(uint32_t)) {
		merror(msg);
		return;
	}

	/* extract data */
	entry = ((uint32_t*) msg->data)[0];

	/* get requested directory */
	dir = vfs_get_index(RP_INDEX(msg->target));

	if (dir) {
		mutex_spin(&dir->mutex);

		if (dir->type & RP_TYPE_DIR) {

			/* check permissions */
			if ((acl_get(dir->acl, gettuser()) & PERM_READ) == 0) {
				err = 1;
			}
			else if (vfs_dir_list(dir, entry, buffer, 100)) {
				/* return ERR_FILE if the entry does not exist */
				err = 1;
			}
		}
		else {
			/* return ERR_TYPE if <dir> is not a directory */
			err = 1;
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
		msg->length = strlen(buffer) + 1;
		strcpy((char*) msg->data, buffer);
		mreply(msg);
	}
}
