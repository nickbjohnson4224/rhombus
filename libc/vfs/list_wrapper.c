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

#include <string.h>
#include <stdlib.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

/*****************************************************************************
 * __list_rcall_wrapper
 */

char *__list_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv) {
	struct vfs_obj *dir;
	uint32_t entry;
	char *name;

	if (argc <= 1) {
		return NULL;
	}

	/* find directory */
	dir = vfs_get(index);

	if (!dir) {
		return strdup("! nfound");
	}

	entry = atoi(argv[1]);

	mutex_spin(&dir->mutex);

	if (!(dir->type & FS_TYPE_DIR)) {
		mutex_free(&dir->mutex);
		return strdup("! notdir");
	}

	if ((acl_get(dir->acl, gettuser()) & PERM_READ) == 0) {
		mutex_free(&dir->mutex);
		return strdup("! denied");
	}

	name = vfs_list(dir, entry);

	if (!name) {
		mutex_free(&dir->mutex);
		return strdup("! nfound");
	}

	mutex_free(&dir->mutex);

	return name;
}
