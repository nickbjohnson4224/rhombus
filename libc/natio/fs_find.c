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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_find
 *
 * Finds the filesystem object with the given path <path> from <root> if it 
 * exists. If it does not exist, this function returns 0.
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

uint64_t fs_find(uint64_t root, const char *path) {
	struct msg *msg;
	uint64_t rp;
	uint32_t err;
	char *path_s;

	if (!root) {
		root = fs_root;
	}

	path_s = path_simplify(path);
	if (!path_s) {
		return 0;
	}
	
	msg = aalloc(sizeof(struct msg) + 1 + strlen(path_s) + 1, PAGESZ);
	if (!msg) return 0;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = root;
	msg->length = strlen(path_s) + 2;
	msg->port   = PORT_FIND;
	msg->arch   = ARCH_NAT;
	msg->data[0] = 0;
	strcpy((char*) &msg->data[1], path_s);
	free(path_s);

	if (msend(msg)) {
		errno = ENOSYS;
		return 0;
	}
	msg = mwait(PORT_REPLY, root);

	if (msg->length < sizeof(uint64_t) + sizeof(uint32_t)) {
		free(msg);
		return 0;
	}

	rp  = ((uint64_t*) msg->data)[0];
	err = ((uint32_t*) msg->data)[2];

	free(msg);

	if (err) {	
		switch (err) {
		case 1:  errno = ENOENT; break;
		case 2:  errno = EACCES; break;
		case 3:  errno = ENOSYS; break;
		default: errno = EUNK;   break;
		}
		return 0;
	}

	return rp;
}
