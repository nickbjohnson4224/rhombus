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

#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * rp_open
 *
 * Indicates to the driver that the resource <rp> is in use by this process.
 * Returns zero on success, nonzero on error.
 */

int rp_open(rp_t a, rp_t b, int status) {
	char *reply;

	reply = rcalls(b, a, "open %d", status);

	if (!reply) {
		errno = ENOSYS;
		return 1;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nosys"))		errno = ENOSYS;
		else if (!strcmp(reply, "! denied"))	errno = EACCES;
		else if (!strcmp(reply, "! nfound"))	errno = ENOENT;
		else									errno = EUNK;
		free(reply);
		return 1;
	}

	free(reply);
	return 0;
}

/*****************************************************************************
 * fs_open
 *
 * Attempt to open the resource at <path>. Returns a pointer to the opened
 * resource on success, zero on error.
 */

rp_t fs_open(const char *path, int status) {
	uint64_t rp;

	rp = fs_find(path);

	if (!rp) {
		return 0;
	}

	if (rp_open(RP_CURRENT_THREAD, rp, status)) {
		return 0;
	}

	return rp;
}

/*****************************************************************************
 * fs_open
 *
 * Attempt to open the resource at <path>. Returns a pointer to the opened
 * resource on success, zero on error.
 */

rp_t fs_openh(const char *path, int status) {
	uint64_t rp;

	rp = fs_find(path);

	if (!rp) {
		return 0;
	}

	if (rp_open(RP_CURRENT_PROC, rp, status)) {
		return 0;
	}

	return rp;
}
