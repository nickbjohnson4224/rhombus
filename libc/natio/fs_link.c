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
 * fs_link
 *
 * Creates a new link (also known as a hard link) to the existing file <link>
 * at path <path>. <link> may not be a directory, and must be a file in the
 * same driver as the new link or be zero. If <link> is zero, the link is 
 * removed instead of created. Returns zero on success, nonzero on error.
 */

int fs_link(const char *path, const char *link) {
	uint64_t dir;
	uint64_t rp;
	char *parent;
	char *name;
	int err;

	parent = path_parent(path);
	name   = path_name(path);

	dir = fs_find(parent);
	rp  = fs_find(link);

	err = rp_link(dir, name, rp);

	free(parent);
	free(name);

	return err;
}

int fs_ulink(const char *path) {
	return fs_link(path, "@0:0");
}

int rp_link(uint64_t dir, const char *name, uint64_t link) {
	char *reply;

	if (!name) {
		return 1;
	}

	if (link && RP_PID(dir) != RP_PID(link)) {
		return 1;
	}

	reply = rcallf(dir, "fs_link %s %r", name, link);

	if (!reply) {
		errno = ENOSYS;
		return 1;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound"))   errno = ENOENT;
		else if (!strcmp(reply, "! denied"))   errno = EACCES;
		else if (!strcmp(reply, "! type"))     errno = ENOTDIR;
		else if (!strcmp(reply, "! notempty")) errno = ENOTEMPTY;
		else                                   errno = EUNK;
		free(reply);
		return 1;
	}

	free(reply);
	return 0;
}

/*****************************************************************************
 * fs_plink
 *
 * Sets the link at <link> to point to the filesystem object <fobj>. Returns 
 * zero on success, nonzero on failure.
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

int fs_plink(uint64_t link, uint64_t fobj) {
	struct msg *msg;
	int err;

	msg = aalloc(sizeof(struct msg) + sizeof(uint64_t), PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = link;
	msg->length = sizeof(uint64_t);
	msg->port   = PORT_LINK;
	msg->arch   = ARCH_NAT;

	((uint64_t*) msg->data)[0] = fobj;

	if (msend(msg)) return 1;
	msg = mwait(PORT_REPLY, link);

	if (msg->length < sizeof(uint8_t)) {
		free(msg);
		return 1;
	}

	err = msg->data[0];

	free(msg);
	return err;
}
