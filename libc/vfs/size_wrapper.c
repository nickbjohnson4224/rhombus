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

#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <natio.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __size_rcall_wrapper
 */

char *__size_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	if ((acl_get(r->acl, gettuser()) & PERM_READ) == 0) {
		return strdup("! denied");
	}

	if ((r->type & FS_TYPE_FILE) == 0 || (r->type & FS_TYPE_CHAR) != 0) {
		return strdup("! type");
	}

	return saprintf("%d:%d", (uint32_t) (r->size >> 32), (uint32_t) r->size);
}
