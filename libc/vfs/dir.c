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
#include <proc.h>
#include <vfs.h>

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

/*****************************************************************************
 * vfs_pull
 *
 * Remove the filesystem object <robj> from its parent directory. Calls the 
 * active driver's pull function and returns zero on success; returns nonzero 
 * on error.
 */

int vfs_pull(uint64_t source, struct resource *robj) {
	struct vfs_node *obj;

	return 1;

	if (!robj) {
		return 1;
	}

	obj = robj->vfs;

	mutex_spin(&obj->mutex);

	free(obj->name);
	
	if (obj->mother) {
		mutex_spin(&obj->mother->mutex);
		if (obj->mother->daughter == obj) {
			obj->mother->daughter = obj->sister1;
			if (obj->sister1) {
				obj->sister1->sister0 = NULL;
			}
		}
		else {
			if (obj->sister0) {
				obj->sister0->sister1 = obj->sister1;
			}
			if (obj->sister1) {
				obj->sister1->sister0 = obj->sister0;
			}
		}
		mutex_free(&obj->mother->mutex);
	}

	mutex_free(&obj->mutex);

	if (_vfs_sync) {
		return _vfs_sync(source, robj);
	}
	else {
		return 0;
	}
}

/*****************************************************************************
 * vfs_list
 *
 * Return a copy of the <entry>th entry in the directory <dir>. Returns null
 * on error.
 */

char *vfs_list(struct vfs_node *dir, int entry) {
	struct vfs_node *daughter;

	if (!dir) {
		return NULL;
	}

	daughter = dir->daughter;

	/* select the <entry>th daughter node */
	while (daughter) {
		if (entry <= 0) {
			break;
		}
		else {
			daughter = daughter->sister1;
			entry--;
		}
	}

	if (daughter) {
		/* return name of selected daughter */
		return strdup(daughter->name);
	}
	else {
		return NULL;
	}
}
