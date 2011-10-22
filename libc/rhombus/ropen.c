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

#include <rhombus.h>

/*****************************************************************************
 * ropen
 *
 * If <fd> is non-negative, the file descriptor <fd> is set to point to <rp>
 * and have mode <mode>. If there is already a connection open for <fd>, it
 * is closed (but in a way such that if only the connection mode is changed,
 * the connection is never fully closed.)
 *
 * If <fd> is negative, a new file descriptor is allocated with a connection
 * to <rp> with status <mode>.
 *
 * Returns the modified file descriptor on success, negative on error.
 */

int ropen(int fd, rp_t rp, int mode) {
	int old_mode;
	int err;
	rp_t old_rp;

	if (!rp) {
		// simply clear file descriptor
		return fd_set(fd, RP_NULL, 0);
	}

	if (fd < 0) {
		// new file descriptor requested
		fd = fd_alloc();
	}

	mode |= STAT_OPEN;

	old_mode = fd_mode(fd);
	old_rp = fd_rp(fd);

	err = 0;
	if (old_rp == rp) {
		// open new connection
		err += rp_setstat(rp, mode);

		// close old connection
		err += rp_clrstat(rp, old_mode &~ mode);
	}
	else {

		// open new connection
		err += rp_setstat(rp, mode);

		// close old connection
		if (old_rp) err += rp_clrstat(old_rp, old_mode);
	}

	if (err) {
		return -err;
	}

	return (fd_set(fd, rp, mode)) ? -1 : fd;
}
