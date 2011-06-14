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

#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <errno.h>
#include <proc.h>

int rp_lock_acquire(uint64_t rp, int locktype) {
	char *reply;

	reply = rcallf(rp, "fs_setlock %d", locktype);

	if (!reply) {
		errno = ENOSYS;
		return 1;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! denied")) errno = EACCES;
		else if (!strcmp(reply, "! locked")) errno = EAGAIN;
		else                                 errno = EUNK;
		free(reply);
		return 1;
	}

	free(reply);
	return 0;
}

int rp_lock_waitfor(uint64_t rp, int locktype) {
	
	while (1) {
		if (!rp_lock_acquire(rp, locktype)) {
			return 0;
		}
		
		if (errno == ENOSYS || errno == ENOENT || errno == EACCES) {
			return 1;
		}

		sleep();
	}
}

int rp_lock_current(uint64_t rp) {
	int locktype;
	char *reply;

	reply = rcall(rp, "fs_getlock");

	if (!reply) {
		errno = ENOSYS;
		return LOCK_NO;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! denied")) errno = EACCES;
		else if (!strcmp(reply, "! locked")) errno = EAGAIN;
		else                                 errno = EUNK;
		free(reply);
		return LOCK_NO;
	}

	locktype = atoi(reply);

	free(reply);
	return locktype;
}
