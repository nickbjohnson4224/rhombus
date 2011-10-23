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

#include <rhombus.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <rho/natio.h>

/*****************************************************************************
 * fs_find
 *
 * Finds the robject with the given path <path> if it exists and returns a
 * robject pointer to it. If it does not exist, this function returns RP_NULL.
 */

static rp_t __fs_find(const char *path, int linkmax, int link);

rp_t fs_find(const char *path) {
	return __fs_find(path, 64, 0);
}

rp_t fs_findl(const char *path) {
	return __fs_find(path, 64, 1);
}

static rp_t __fs_find(const char *path, int linkmax, int link) {
	uint64_t rp;
	uint64_t root;
	char *reply;
	char *path_s;

	// if too many links have been followed, fail
	if (linkmax < 0) {
		errno = ELOOP;
		return RP_NULL;
	}

	// if path is NULL, return NULL
	if (!path) {
		errno = ENOENT;
		return RP_NULL;
	}

	// if preceeded by a resource pointer, use that as root and strip it
	if (path[0] == '@') {
		root = ator(path);
		while (*path != '/' && *path) path++;
	}
	else {
		root = fs_root;
	}

	// simply return root if path is nonexistent
	if (path[0] == '\0' || (path[0] == '/' && path[1] == '\0')) {
		return root;
	}

	path_s = path_simplify(path);
	if (!path_s) return RP_NULL;

	if (link) {
		reply = rcall(root, "find -L %s", path_s);
	}
	else {
		reply = rcall(root, "find %s", path_s);
	}

	free(path_s);

	if (!reply) {
		errno = ENOSYS;
		return RP_NULL;
	}

	if (iserror(reply)) {
		errno = geterror(reply);
		free(reply);
		return RP_NULL;
	}

	if (reply[0] == '>' && reply[1] == '>' && reply[2] == ' ') {
		rp = __fs_find(&reply[3], linkmax - 1, link);
		free(reply);
		return rp;
	}	

	rp = ator(reply);
	free(reply);

	return rp;
}
