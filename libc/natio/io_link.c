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
 * io_link
 *
 * High level filesystem operation. Modifies the link pointer of an existing
 * link or directory at path <name> to resource pointer <rp>. If <rp> is null,
 * the link becomes a directory. Returns zero on success, nonzero on error.
 */

int io_link(const char *name, uint64_t rp) {
	uint64_t link;

	/* find actual link */
	link = fs_lfind(RP_NULL, name);

	if (!link) {
		/* try to create a new link */
		link = io_cons(name, FOBJ_DIR);

		if (!link) {
			return 1;
		}
	}

	/* set link value */
	return fs_link(link, rp);
}
