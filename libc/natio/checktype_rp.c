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
 * checktype_rp
 *
 * Returns true iff the robject <rp> is a member of the type class <type>.
 */

bool checktype_rp(rp_t rp, const char *type) {
	char *reply;
	bool is_type;
	
	if (!rp) {
		return false;
	}

	reply = rcall(rp, "type");

	if (!reply) {
		errno = ENOSYS;
		return false;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! nosys"))  errno = ENOSYS;
		else                                 errno = EUNK;
		free(reply);
		return false;
	}

	is_type = strstr(reply, type) ? true : false;
	free(reply);

	return is_type;
}
