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

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/access.h>

#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <natio.h>

/*****************************************************************************
 * __rdi_symlink_handler
 */

char *__rdi_symlink_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	char *symlink;

	if (argc < 2) {
		return NULL;
	}

	r = index_get(index);
	if (!r) {
		/* link not found */
		return strdup("! nfound");
	}

	/* get arguments */
	symlink = argv[1];

	mutex_spin(&r->mutex);
	
	if (!FS_IS_LINK(r->type)) {
		/* not a link */
		mutex_free(&r->mutex);
		return strdup("! type");
	}

	if (!vfs_permit(r, source, PERM_WRITE)) {
		/* access denied */
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	/* replace symlink contents */
	if (r->symlink) free(r->symlink);
	r->symlink = strdup(symlink);

	/* notify driver of change */
	if (__rdi_callback_lnksync) __rdi_callback_lnksync(r);

	mutex_free(&r->mutex);

	return strdup("T");
}
