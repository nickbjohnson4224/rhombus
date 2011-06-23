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
#include <rdi/vfs.h>

/*****************************************************************************
 * index_lookup
 *
 * Inode lookup hashtable (using chaining). Used to find filesystem objects
 * by index alone.
 */

static struct resource *index_lookup[256];
static bool m_index_lookup;

/*****************************************************************************
 * index_get
 *
 * Find a resource by index number. Returns the found resource structure on 
 * success, NULL on error.
 */

struct resource *index_get(uint32_t index) {
	struct resource *r;

	mutex_spin(&m_index_lookup);

	r = index_lookup[index % 256];

	while (r) {
		if (r->index == index) {
			mutex_free(&m_index_lookup);
			return r;
		}
		r = r->next;
	}

	mutex_free(&m_index_lookup);

	return NULL;
}

/*****************************************************************************
 * index_set
 *
 * Register a resource with an index. If another resource already uses that 
 * index, its index is set to zero and a pointer to it is returned. The index 
 * field of the registered resource will be set to the appropriate index.
 */

struct resource *index_set(uint32_t index, struct resource *r) {
	struct resource *old;

	/* check for existing resource */
	old = index_get(index);

	/* remove existing resource if found */
	if (old) {
		mutex_spin(&m_index_lookup);

		if (old->next) {
			old->next->prev = old->prev;
		}

		if (old->prev) {
			old->prev->next = old->next;
		}
		else {
			index_lookup[old->index % 256] = NULL;
		}

		mutex_free(&m_index_lookup);

		old->next = old->prev = NULL;
		old->index = 0;
	}

	/* add new resource if it exists */
	if (r) {
		r->index = index;
		mutex_spin(&m_index_lookup);
		mutex_spin(&r->mutex);
		r->next = index_lookup[index % 256];
		r->prev = NULL;
	
		if (r->next) {
			r->next->prev = r;
		}

		index_lookup[index % 256] = r;
		mutex_free(&r->mutex);
		mutex_free(&m_index_lookup);
	}

	/* return old resource if found */
	return old;
}
