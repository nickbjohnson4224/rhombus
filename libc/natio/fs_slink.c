/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
 * fs_slink
 *
 * Creates a new symbolic link to the path <link> at the path <path>. If this
 * symbolic link already exists, it's path is updated. Returns zero on 
 * success, nonzero on error.
 */

int fs_slink(const char *path, const char *link) {
	uint64_t rp;

	rp = fs_find(path);
	if (!rp) rp = fs_cons(path, "link");

	return rp_slink(rp, link);
}

int fs_plink(const char *path, uint64_t link_rp, const char *link_path) {
	char *link_path1;
	int err;

	if (link_rp && link_path) link_path1 = saprintf("%r/%s", link_rp, link_path);
	else if (link_path) link_path1 = strdup(link_path);
	else if (link_rp) link_path1 = rtoa(link_rp);

	err = fs_slink(path, link_path1);
	free(link_path1);

	return err;
}

int rp_plink(uint64_t rp, uint64_t link_rp, const char *link_path) {
	char *link_path1;
	int err;

	if (link_rp && link_path) link_path1 = saprintf("%r/%s", link_rp, link_path);
	else if (link_path) link_path1 = strdup(link_path);
	else if (link_rp) link_path1 = rtoa(link_rp);

	err = rp_slink(rp, link_path1);
	free(link_path1);

	return err;
}

int rp_slink(uint64_t rp, const char *link) {
	char *reply;

	reply = rcall(rp, "symlink %s", link);

	if (!reply) {
		errno = ENOSYS;
		return 1;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! denied")) errno = EACCES;
		else if (!strcmp(reply, "! type"))   errno = EINVAL;
		else                                 errno = EUNK;
		free(reply);
		return 1;
	}

	free(reply);
	return 0;
}
