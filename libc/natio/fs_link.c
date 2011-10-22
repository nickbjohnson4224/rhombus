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
#include <errno.h>

#include <rho/natio.h>

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

	rp  = fs_find(link);
	
	dir = fs_find(parent);
	rp_setstat(dir, STAT_WRITER);
	err = rp_link(dir, name, rp);
	rp_clrstat(dir, STAT_WRITER);

	free(parent);
	free(name);

	return err;
}

int rp_ulink(rp_t dir, const char *name) {
	char *reply;

	if (!name) {
		return 1;
	}

	reply = rcall(dir, "unlink %s", name);

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

int fs_ulink(const char *path) {
	uint64_t dir;
	char *parent;
	char *name;
	int err;

	parent = path_parent(path);
	name   = path_name(path);

	dir = fs_find(parent);
	rp_setstat(dir, STAT_WRITER);
	err = rp_ulink(dir, name);
	rp_clrstat(dir, STAT_WRITER);

	free(parent);
	free(name);

	return err;
}

int rp_link(uint64_t dir, const char *name, uint64_t link) {
	char *reply;

	if (!name) {
		return 1;
	}

	if (link && RP_PID(dir) != RP_PID(link)) {
		return 1;
	}

	reply = rcall(dir, "link %s %r", name, link);

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
