/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <driver.h>
#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>

/*****************************************************************************
 * index_lookup
 *
 * Inode lookup hashtable (using chaining). Used to find filesystem objects
 * by index alone.
 */

static struct vfs_obj *index_lookup[256];
static bool m_index_lookup;

/*****************************************************************************
 * vfs_get_index
 *
 * Find a virtual filesystem object by index number. Returns the found 
 * filesystem object on success, NULL on error.
 */

struct vfs_obj *vfs_get_index(uint32_t index) {
	struct vfs_obj *obj;

	mutex_spin(&m_index_lookup);

	obj = index_lookup[index % 256];

	while (obj) {
		if (obj->index == index) {
			mutex_free(&m_index_lookup);
			return obj;
		}
		obj = obj->next;
	}

	mutex_free(&m_index_lookup);

	return NULL;
}

/*****************************************************************************
 * vfs_set_index
 *
 * Register a virtual filesystem object with an index. If another filesystem
 * object already uses that index, its index is set to zero and a pointer to
 * it is returned. The index field of the registered object will be set to the
 * appropriate index.
 */

struct vfs_obj *vfs_set_index(uint32_t index, struct vfs_obj *obj) {
	struct vfs_obj *old_obj;

	/* check for existing object */
	old_obj = vfs_get_index(index);

	/* remove existing object if found */
	if (old_obj) {
		mutex_spin(&m_index_lookup);

		if (old_obj->next) {
			old_obj->next->prev = old_obj->prev;
		}

		if (old_obj->prev) {
			old_obj->prev->next = old_obj->next;
		}
		else {
			index_lookup[old_obj->index % 256] = NULL;
		}

		mutex_free(&m_index_lookup);

		old_obj->next = old_obj->prev = NULL;
		old_obj->index = 0;
	}

	/* add new object if it exists */
	if (obj) {
		obj->index = index;
		mutex_spin(&m_index_lookup);
		obj->next = index_lookup[index % 256];
		obj->prev = NULL;
	
		if (obj->next) {
			obj->next->prev = obj;
		}

		index_lookup[index % 256] = obj;
		mutex_free(&m_index_lookup);
	}

	/* return old object if found */
	return old_obj;
}
