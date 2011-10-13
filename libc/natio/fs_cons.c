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
 * Attempts to create a new filesystem object of type <type> at the path 
 * <path>. Returns a resource pointer to the new resource on success, RP_NULL 
 * on failure.
 *
 * Notes:
 *
 * <type> should only be "file", "dir", or "link", unless driver intimate
 * driver details are known.
 */

rp_t fs_cons(const char *path, const char *type) {
	char *dirname;
	char *name;
	rp_t dir;
	rp_t rp;

	// check for existing entries
	if (fs_find(path)) {
		errno = EEXIST;
		return 1;
	}

	// find parent directory
	dirname = path_parent(path);
	dir = fs_find(dirname);
	free(dirname);
	if (!dir) return RP_NULL;

	// construct new robject
	rp = rp_cons(dir, type);
	if (!rp) return RP_NULL;
	
	// add to directory
	name = path_name(path);
	if (rp_link(dir, name, rp)) {
		free(name);
		return RP_NULL;
	}

	free(name);
	return rp;
}
