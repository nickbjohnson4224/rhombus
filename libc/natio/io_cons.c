/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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
 * io_cons
 *
 * High level filesystem operation. Attempts to construct a resource of type
 * <type> at the path <name>. Returns a resource pointer to the constructed
 * resource on success, RP_NULL on failure.
 */

uint64_t io_cons(const char *name, int type) {
	uint64_t dir, rp;
	char *dirname, *resname;

	dirname = path_parent(name);
	resname = path_name(name);

	if (!dirname || !resname) {
		return RP_NULL;
	}

	/* find requested parent directory */
	dir = fs_find(RP_NULL, dirname);
	free(dirname);

	if (!dir) {
		free(resname);
		return RP_NULL;
	}

	/* construct file */
	rp = fs_cons(dir, resname, type);
	free(resname);

	return rp;
}
