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
#include <string.h>
#include <errno.h>

/*****************************************************************************
 * rp_type
 *
 * Check whether the resource <rp> implements the type <type>. Returns 1 if
 * it does, 0 otherwise.
 */

int rp_type(rp_t rp, const char *type) {
	char *reply;

	if (!rp) {
		return 0;
	}

	reply = rcall(rp, "type");

	if (iserror(reply)) {
		errno = geterror(reply);
		free(reply);
		return 0;
	}

	if (strstr(reply, type)) {
		free(reply);
		return 1;
	}
	else {
		free(reply);
		return 0;
	}
}
