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
 * rp_cons
 *
 * Attempts to create a new robject of type <type> in the driver <driver>. 
 * Returns a robject pointer to the new robject on success, RP_NULL on
 * failure.
 */

rp_t rp_cons(rp_t driver, const char *type) {
	rp_t rp;
	char *reply;

	reply = rcall(driver, "cons %s", type);

	if (!reply) {
		errno = ENOSYS;
		return RP_NULL;
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
