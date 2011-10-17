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
 * rp_list
 *
 * List the contents of the directory <dir>. On success, a tab-separated list
 * of directory entry names is returned; on failure, NULL is returned.
 */

char *rp_list(rp_t dir) {
	char *reply;

	reply = rcall(dir, "list");
	
	if (!reply) {
		return strdup("");
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! denied")) errno = EACCES;
		else if (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! nosys"))  errno = ENOSYS;
		else if (!strcmp(reply, "! type"))   errno = ENOTDIR;
		else                                 errno = EUNK;
		free(reply);
		return NULL;
	}
	
	return reply;
}
