/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <dict.h>
#include <stdint.h>
#include <stdlib.h>
#include <mutex.h>
#include <string.h>

/****************************************************************************
 * dict_read_rec
 *
 * Recursive dictionary reading function, used by interface read functions.
 * Returns null on failure, pointer to dictionary value on success. Follows
 * all links found. This function is not thread-safe.
 */

static const uint8_t *dict_read_rec
		(struct dict *root, const uint8_t *key, 
		size_t keylen, size_t keypos, size_t *vallen) {

	uint8_t *value;

	if (root->link) {
		value = dict_alloc(1024, true);
		*vallen = 1024;

		dict_link_read(root->link, &key[keypos], keylen - keypos, value, vallen);

		if (!(*vallen)) {
			return NULL;
		}
		
		return value;
	}

	if (keypos >= keylen) {
		*vallen = root->vallen;
		return root->value;
	}
	
	if (root->next[key[keypos]]) {
		return dict_read_rec(root->next[key[keypos]], 
			key, keylen, keypos + 1, vallen);
	}
	else {
		return NULL;
	}
}

/****************************************************************************
 * dict_read
 *
 * Read from dictionary with the given key. Returns null on failure, pointer
 * to dictionary value on success. This function is thread-safe.
 */

const void *dict_read(const void *key, size_t keylen, size_t *vallen) {
	const void *value;
	const uint8_t *bkey = key;

	mutex_spin(&dict_info->mutex);

	value = dict_read_rec(&dict_info->root, bkey, keylen, 0, vallen);

	mutex_free(&dict_info->mutex);

	return value;
}

/****************************************************************************
 * dict_readstr
 *
 * Read from dictionary using the given string as a key. Returns null on
 * failure, pointer to dictionary value on success. This function is thread
 * safe.
 */

const void *dict_readstr(const char *key, size_t *vallen) {
	return dict_read(key, strlen(key), vallen);
}

/****************************************************************************
 * dict_readstrns
 *
 * Read from the dictionary from a given namespace using the given string as
 * a key. Returns null on failure, pointer to dictionary value on success.
 * This function is thread-safe.
 */

const void *dict_readstrns(const char *namespace, 
		const char *key, size_t *vallen) {
	char *buffer;
	const void *val;

	buffer = malloc(strlen(namespace) + strlen(key) + 1);
	strcpy(buffer, namespace);
	strcat(buffer, key);

	val = dict_readstr(buffer, vallen);

	free(buffer);

	return val;
}
