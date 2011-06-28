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
#include <rdi/access.h>

#include <string.h>
#include <mutex.h>

/*****************************************************************************
 * __rdi_size_handler
 *
 * Notes:
 *
 * XXX:
 * This function returns the size as two colon-separated decimal integers,
 * due to the current printf's inability to print 64-bit unsigned integers.
 * When it is capable, this should be fixed.
 */

char *__rdi_size_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint64_t size;

	r = index_get(index);
	if (!r) {
		/* file not found */
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);
	if (!vfs_permit(r, source, PERM_READ)) {
		/* permission denied */
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	if (!FS_IS_FILE(r->type) || (r->type & FS_TYPE_CHAR) != 0) {
		/* is not a file or is a character file */
		mutex_free(&r->mutex);
		return strdup("! type");
	}

	size = r->size;
	mutex_free(&r->mutex);

	return saprintf("%d:%d", (uint32_t) (size >> 32), (uint32_t) size);
}
