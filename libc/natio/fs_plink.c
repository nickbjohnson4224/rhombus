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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <rho/natio.h>

/*****************************************************************************
 * fs_plink
 *
 * Creates a new symbolic link to the path <link_path> from the robject 
 * pointer <link_rp> at the path <path>. If this symbolic link already exists,
 * its path is updated. Returns zero on success, nonzero on error.
 */

int fs_plink(const char *path, rp_t link_rp, const char *link_path) {
	char *link_path1;
	int err;

	if (link_rp && link_path) {
		link_path1 = saprintf("%r/%s", link_rp, link_path);
	}
	else if (link_path) {
		link_path1 = strdup(link_path);
	}
	else if (link_rp) {
		link_path1 = rtoa(link_rp);
	}
	else {
		return 1;
	}

	err = fs_slink(path, link_path1);
	free(link_path1);

	return err;
}
