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
 * vfs_link
 *
 * Create a hard link in the directory <dir> that points to the resource <r>,
 * giving the hard link entry the name <name>. If the specified directory 
 * entry already exists, it is modified to point to <r>. Returns zero on 
 * success, nonzero on error.
 *
 * This function does not acquire a lock on the given directory or resource,
 * but requires that neither be modified while it is running.
 */

int vfs_link(struct vfs_node *dir, const char *name, struct resource *r) {
	struct vfs_node *entry;
	struct vfs_node *sister;

	if (FS_IS_DIR(r->type) && r->vfs_refcount > 0) {
		/* directory has already been linked: fail */
		return 1;
	}

	/* create new directory entry */
	entry = calloc(sizeof(struct vfs_node), 1);
	entry->mother   = dir;
	entry->name     = strdup(name);
	entry->resource = r;

	/* place entry into directory */
	sister = dir->daughter;

	if (!sister) {
		/* no other entries: insert directly */
		dir->daughter = entry;
	}
	else {
		/* find the proper place in entry the list to insert */
		while (sister->sister1) {
			if (strcmp(sister->name, name) > 0) {
				break;
			}
			else if (strcmp(sister->name, name) == 0) {
				break;
			}
			sister = sister->sister1;
		}

		/* check if entry exists */
		if (strcmp(sister->name, name) == 0) {
		
			/* free new entry */
			free(entry->name);
			free(entry);
			entry = sister;

			/* decrease displaced resource reference count */
			entry->resource->vfs_refcount--;

			/* modify old entry */
			entry->resource->vfs = NULL;
			entry->resource = r;
		}

		/* insert into list */
		else if (strcmp(sister->name, name) > 0) {
		
			/* insert before sister */
			if (sister->sister0) {
				/* link to sister->sister0 */
				sister->sister0->sister1 = entry;
			}
			else {
				/* first entry: new daughter */
				dir->daughter = entry;
			}

			entry->sister0 = sister->sister0;
			entry->sister1 = sister;
			sister->sister0 = entry;
		}
		else {

			/* last entry: insert after sister */
			sister->sister1 = entry;
			entry->sister0 = sister;
		}
	}

	if (FS_IS_DIR(r->type)) {
		/* resource is a directory: add link back */
		r->vfs = entry;
	}
	else {
		/* resource is a file: don't link back */
		r->vfs = NULL;
	}

	/* increase resource reference count */
	r->vfs_refcount++;

	return 0;
}
