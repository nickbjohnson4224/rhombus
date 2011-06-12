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
 * fs_setperm
 *
 * Sets the permissions of the filesystem object <fobj> to <perm> for user
 * <user>. Returns zero on success, nonzero on error.
 */

int rp_setperm(uint64_t fobj, uint32_t user, uint8_t perm) {
	char *reply;

	if (!fobj) {
		return 1;
	}

	reply = rcallf(fobj, "fs_setperm %d %d", user, perm);

	if (!reply) {
		errno = ENOSYS;
		return 1;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! denied")) errno = EACCES;
		else if (!strcmp(reply, "! nosys"))  errno = ENOSYS;
		else                                 errno = EUNK;
		free(reply);
		return 1;
	}
	else {
		errno = 0;
	}

	free(reply);

	return 0;
}

int fs_setperm(const char *path, uint32_t user, uint8_t perm) {
	return rp_setperm(fs_find(path), user, perm);
}
