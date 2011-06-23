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

#include <rdi/vfs.h>

#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * vfs_unlink
 *
 * Removes the directory entry <entry> from its parent directory and frees it. 
 * Returns zero on success, nonzero on error.
 *
 * This function does not acquire a lock on either the given entry, its parent
 * or its resource structure, but all three should not be modified while it
 * is running.
 */

int vfs_unlink(struct vfs_node *dir, const char *name) {
	struct vfs_node *entry;

	if (!dir || !dir->daughter) {
		/* entry is not in a directory at all: fail */
		return 1;
	}

	/* find entry */
	entry = dir->daughter;
	while (entry->sister1) {
		if (strcmp(entry->name, name) == 0) {
			break;
		}
		entry = entry->sister1;
	}

	if (strcmp(entry->name, name)) {
		/* entry is not in the directory */
		return 2;
	}

	if (entry->daughter) {
		/* entry is a non-empty directory: fail */
		return 3;
	}

	/* remove entry from directory */
	if (entry->sister0) {
		/* entry is not first */
		entry->sister0->sister1 = entry->sister1;
		if (entry->sister1) {
			entry->sister1->sister0 = entry->sister0;
		}
	}
	else {
		/* entry is first */
		entry->mother->daughter = entry->sister1;
		if (entry->sister1) {
			entry->sister1->sister0 = NULL;
		}
	}

	/* decrese resource reference count */
	entry->resource->vfs_refcount--;

	/* free entry */
	entry->resource->vfs = NULL;
	free(entry->name);
	free(entry);

	return 0;
}


