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
#include <stdio.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_find
 *
 * Finds the filesystem object with the given path <path> from <root> if it
 * exists. If it does not exist, this function returns 0.
 */

uint64_t fs_find(uint64_t root, const char *path) {
	uint64_t rp;
	char *reply;
	char *path_s;

	path_s = path_simplify(path);
	if (!path_s) return 0;
	if (!root) root = fs_root;

	reply = rcallf(root, "fs_find %s", path_s);
	free(path_s);

	if (!reply) {
		errno = ENOSYS;
		return 0;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound"))	errno = ENOENT;
		else if (!strcmp(reply, "! denied"))	errno = EACCES;
		else if (!strcmp(reply, "! nosys"))		errno = ENOSYS;
		else 									errno = EUNK;
		free(reply);
		return 0;
	}

	rp = ator(reply);
	free(reply);

	return rp;
}
