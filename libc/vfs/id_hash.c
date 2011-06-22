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
#include <mutex.h>
#include <vfs.h>

#define HASHSIZE 127

void id_hash_set(struct id_hash *h, uint32_t id, uint32_t value) {
	struct id_hash_list *node;

	mutex_spin(&h->mutex);

	if (h->size == 0) {
		h->size = HASHSIZE;
		h->table = calloc(sizeof(struct id_hash_list*), HASHSIZE);
	}

	node = malloc(sizeof(struct id_hash_list));
	node->id    = id;
	node->value = value;

	id = id % h->size;
	node->next = h->table[id];
	node->prev = NULL;
	h->table[id] = node;

	mutex_free(&h->mutex);
}

uint32_t id_hash_get(struct id_hash *h, uint32_t id) {
	struct id_hash_list *node;
	
	mutex_spin(&h->mutex);

	if (h->size == 0) {
		mutex_free(&h->mutex);
		return h->nil;
	}

	node = h->table[id % h->size];

	while (node) {
		if (node->id == id) {
			break;
		}
		node = node->next;
	}

	mutex_free(&h->mutex);

	if (!node) {
		return h->nil;
	}
	else {
		return node->value;
	}
}

int id_hash_test(struct id_hash *h, uint32_t id) {
	struct id_hash_list *node;
	
	mutex_spin(&h->mutex);

	if (h->size == 0) {
		mutex_free(&h->mutex);
		return 0;
	}

	node = h->table[id % h->size];

	while (node) {
		if (node->id == id) {
			break;
		}
		node = node->next;
	}

	mutex_free(&h->mutex);

	if (!node) {
		return 0;
	}
	else {
		return 1;
	}
}

int id_hash_count(struct id_hash *h) {
	int count;

	mutex_spin(&h->mutex);
	count = h->count;
	mutex_free(&h->mutex);

	return count;
}

void id_hash_free(struct id_hash *h) {
	
	if (h->table) {
		free(h->table);
	}
}
