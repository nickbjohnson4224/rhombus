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
 * rp_close
 */

int rp_close(rp_t a, rp_t b) {
	char *reply;

	reply = rcalls(b, a, "close");

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

	rtab_close(a, b);
	ftab_set(a, b, 0);
	return 0;
}
