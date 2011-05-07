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
 * vfs_push
 *
 * Add the filesystem object <obj> to the directory <dir>, giving it the
 * name <name>. Calls the active driver's push function and returns zero on 
 * success; returns nonzero on error.
 */

int vfs_push(uint64_t source, struct vfs_obj *dir, struct vfs_obj *obj) {
	struct vfs_obj *sister;

	if (!(dir && obj)) {
		return 1;
	}

	mutex_spin(&dir->mutex);

	obj->mutex    = 0;
	obj->mother   = dir;
	obj->daughter = NULL;
	
	sister = dir->daughter;

	if (!sister) {
		/* only in the list, insert at top */
		dir->daughter = obj;
		obj->sister0 = NULL;
		obj->sister1 = NULL;
	}
	else {
		while (sister->sister1) {
			if (strcmp(sister->name, obj->name) > 0) {
				break;
			}
			sister = sister->sister1;
		}

		/* insert */
		if (strcmp(sister->name, obj->name) > 0) {
			if (sister->sister0) {
				sister->sister0->sister1 = obj;
			}
			else {
				dir->daughter = obj;
			}

			obj->sister0 = sister->sister0;
			obj->sister1 = sister;
			sister->sister0 = obj;
		}
		else {
			sister->sister1 = obj;
			obj->sister0 = sister;
			obj->sister1 = NULL;
		}
	}

	mutex_free(&dir->mutex);

	/* add to index */
	vfs_set(obj->index, obj);

	if (_vfs_push) {
		return _vfs_push(source, obj);
	}
	else {
		return 0;
	}
}

/*****************************************************************************
 * vfs_pull
 *
 * Remove the filesystem object <obj> from its parent directory. Calls the 
 * active driver's pull function and returns zero on success; returns nonzero 
 * on error.
 */

int vfs_pull(uint64_t source, struct vfs_obj *obj) {

	if (!obj) {
		return 1;
	}

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

	if (_vfs_pull) {
		return _vfs_pull(source, obj);
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

char *vfs_list(struct vfs_obj *dir, int entry) {
	struct vfs_obj *daughter;

	if (!dir) {
		return NULL;
	}

	if (entry < 0) {
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
