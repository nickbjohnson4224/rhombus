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
#include <stdlib.h>

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

	if (!rp) {
		// simply clear file descriptor
		fd_set(fd, RP_NULL, 0);

		return fd;
	}

	if (fd < 0) {
		// new file descriptor requested
		fd = fd_alloc();
	}

	if (fd_set(fd, rp, mode)) {
		return -1;
	}

	fd_pullkey(fd);

	if (mode & ACCS_EVENT) {
		// register for events
		free(rcall(rp, fd_getkey(fd, AC_EVENT), "listen"));
	}

	return fd;
}
