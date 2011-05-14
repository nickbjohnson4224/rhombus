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
 * fs_cons
 *
 * Attempts to create a new filesystem object of type <type> and name <name> 
 * in directory <dir>. Returns the new object on success, NULL on failure.
 */

uint64_t fs_cons(uint64_t dir, const char *name, int type) {
	uint32_t pid, index;
	uint64_t rp;
	char *reply;

	if (!dir) {
		return 0;
	}

	reply = rcallf(dir, "fs_cons %s %d", name, type);

	if (!reply) {
		errno = ENOSYS;
		return 0;
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

	if (sscanf(reply, "%i %i", &pid, &index) != 2) {
		free(reply);
		return 0;
	}
	rp = RP_CONS(pid, index);

	free(reply);
	return rp;
}
