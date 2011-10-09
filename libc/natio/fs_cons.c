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
 * fs_cons
 *
 * Attempts to create a new filesystem object of type <type> at the path 
 * <path>. Returns a resource pointer to the new resource on success, NULL on 
 * failure.
 *
 * Notes:
 *
 * <type> should only be FS_TYPE_FILE, FS_TYPE_DIR, or FS_TYPE_FILE | 
 * FS_TYPE_LINK. The driver will fill in the appropriate flags for its files
 * beyond that.
 */

uint64_t fs_cons(const char *path, const char *type) {
	uint64_t dir;
	uint64_t rp;
	char *dirname, *name;

	/* check for existing files */
	if (fs_find(path)) {
		errno = EEXIST;
		return 1;
	}

	/* find parent directory */
	dirname = path_parent(path);

	dir = fs_find(dirname);
	free(dirname);

	if (!dir) return RP_NULL;

	rp = rp_cons(dir, type);
	if (!rp) return RP_NULL;
	
	name = path_name(path);

	if (rp_link(dir, name, rp)) {
		free(name);
		return RP_NULL;
	}

	free(name);
	return rp;
}

uint64_t rp_cons(uint64_t driver, const char *type) {
	uint64_t rp;
	char *reply;

	reply = rcall(driver, "cons %s", type);

	if (!reply) {
		errno = ENOSYS;
		return 0;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound"))	errno = ENOENT;
		else if (!strcmp(reply, "! denied"))	errno = EACCES;
		else if (!strcmp(reply, "! nosys"))		errno = ENOSYS;
		else if (!strcmp(reply, "! construct")) errno = ENOSPC;
		else if (!strcmp(reply, "! type"))		errno = ENOTDIR;
		else 									errno = EUNK;
		free(reply);
		return 0;
	}

	rp = ator(reply);
	free(reply);

	return rp;
}
