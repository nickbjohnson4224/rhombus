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
#include <natio.h>
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

/*****************************************************************************
 * __rdi_link_handler
 */

char *__rdi_link_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *dir;
	struct resource *r;
	const char *name;
	uint64_t link;
	int err;

	if (argc < 3) {
		return NULL;
	}

	/* find directory */
	dir = index_get(index);
	if (!dir) return strdup("! nfound");

	if (!FS_IS_DIR(dir->type) || !dir->vfs) {
		/* dir is not a directory */
		return strdup("! type");
	}

	if (!vfs_permit(dir, source, PERM_WRITE)) {
		/* permission denied */
		return strdup("! denied");
	}

	/* parse arguments */
	name = argv[1];
	link = ator(argv[2]);

	if (link) {
		/* create new link */
		
		if (RP_PID(link) != getpid()) {
			/* link is outside current driver */
			return strdup("! extern");
		}

		/* find resource to be linked */
		r = index_get(RP_INDEX(link));
		if (!r) return strdup("! nfound");

		if (FS_IS_DIR(r->type) && r->vfs_refcount) {
			/* cannot link directories that are already linked */
			return strdup("! type");
		}

		/* create link */
		mutex_spin(&dir->vfs->mutex);
		mutex_spin(&r->mutex);
		err = vfs_link(dir->vfs, name, r);
		mutex_free(&r->mutex);

		if (err) {
			mutex_free(&dir->vfs->mutex);
			return strdup("! link");
		}

		/* synchronize with driver */
		mutex_spin(&dir->mutex);
		if (__rdi_callback_dirsync) __rdi_callback_dirsync(dir);
		mutex_free(&dir->mutex);
		mutex_free(&dir->vfs->mutex);

		return strdup("T");
	}
	else {

		/* find linked resource */
		r = vfs_find(dir->vfs, name, NULL);
		
		/* delete link */
		mutex_spin(&dir->vfs->mutex);
		err = vfs_unlink(dir->vfs, name);
		mutex_free(&dir->vfs->mutex);

		switch (err) {
		case 0: break;
		case 1: return strdup("! type");
		case 2: return strdup("! nfound");
		case 3: return strdup("! notempty");
		default: return NULL;
		}

		/* if refcount is zero, free resource */
		if (r->vfs_refcount == 0) {
			/* this should be replaced with a close from source 0 */
//			if (_vfs_free) _vfs_free(source, r);
//			else resource_free(r);
		}

		return strdup("T");
	}
}
