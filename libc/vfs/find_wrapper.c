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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __find_wrapper
 *
 * Finds the requested resource in the virtual filesystem.
 *
 * protocol:
 *   port: PORT_FIND
 *
 *   request:
 *     char link
 *     char path[]
 *
 *   reply:
 *     uint64_t rp
 *     uint32_t err
 *
 *   errors:
 *     0 - none
 *     1 - not found
 *     2 - permission denied
 *     3 - not implemented
 */

void __find_wrapper(struct msg *msg) {
	uint64_t file;
	struct vfs_obj *root;
	const char *path;
	uint8_t link;

	/* check request */
	if (msg->length < 1) {
		merror(msg);
		return;
	}

	/* extract data */
	link = msg->data[0];
	path = (const char*) &msg->data[1];

	/* find root node */
	root = vfs_get(RP_INDEX(msg->target));

	if (!root) {
		merror(msg);
		return;
	}

	/* find pointer to file */
	file = vfs_find(root, path, (link) ? true : false);

	msg->length = sizeof(uint64_t) + sizeof(uint32_t);
	if (file) {
		((uint64_t*) msg->data)[0] = file;
		((uint32_t*) msg->data)[2] = 0;
	}
	else {
		((uint32_t*) msg->data)[2] = 1;
	}
	
	mreply(msg);
}

/*****************************************************************************
 * __find_rcall_wrapper
 */

char *__find_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv) {
	struct vfs_obj *root;
	const char *path;
	uint64_t file;
	bool link;

	if (argc <= 1) {
		return NULL;
	}

	// check for follow link flag
	if (argc == 2) {
		link = false;
		path = argv[1];
	}
	else if (!strcmp(argv[1], "-l")) {
		link = true;
		path = argv[2];
	}
	else {
		return NULL;
	}

	/* find root node */
	root = vfs_get(index);

	if (!root) {
		return strdup("! nfound");
	}

	/* found pointer to file */
	file = vfs_find(root, path, link);

	if (file) {
		return saprintf("%d %d", RP_PID(file), RP_INDEX(file));
	}
	else {
		return saprintf("! nfound");
	}
}
