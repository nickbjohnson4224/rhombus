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
 * dict_write_rec
 *
 * Recursive dictionary writing function, used by interface write functions.
 * Sets the dictionary value to the given value unless the given value is
 * null, in which case the value is deleted. This function is not 
 * thread-safe.
 */

static void dict_write_rec
		(struct dict *root, const uint8_t *key, size_t keylen, size_t keypos,
		const uint8_t *val, size_t vallen) {

	if (root->link) {
		dict_link_write(root->link, &key[keypos], keylen - keypos, val, vallen);

		return;
	}

	if (keypos >= keylen) {
		if (val) {
			root->value = memcpy(dict_alloc(vallen, true), val, vallen);
			root->vallen = vallen;
		}
		else {
			root->value = NULL;
			root->vallen = 0;
		}
		return;
	}
	
	if (!root->next[key[keypos]]) {
		root->next[key[keypos]] = dict_alloc(sizeof(struct dict), false);
	}

	dict_write_rec(root->next[key[keypos]], 
		key, keylen, keypos + 1, val, vallen);
}

/****************************************************************************
 * dict_write
 *
 * Write value to dictionary with the given key pair. This function is 
 * thread-safe.
 */

void dict_write(const void *key, size_t keylen, const void *val, size_t vallen) {

	mutex_spin(&dict_info->mutex);

	dict_write_rec(&dict_info->root, key, keylen, 0, val, vallen);

	mutex_free(&dict_info->mutex);
}

/****************************************************************************
 * dict_writestr
 *
 * Write value to dictionary using the given string as a key. This function 
 * is thread-safe.
 */

void dict_writestr(const char *key, const void *val, size_t vallen) {
	dict_write((uint8_t*) key, strlen(key), val, vallen);
}

/****************************************************************************
 * dict_readstrns
 *
 * Read from the dictionary from a given namespace using the given string as
 * a key. This function is thread-safe.
 */

void dict_writestrns(const char *namespace, const char *key,
		const void *val, size_t vallen) {
	char *buffer;

	buffer = malloc(strlen(namespace) + strlen(key) + 1);
	strcpy(buffer, namespace);
	strcat(buffer, key);

	dict_writestr(buffer, val, vallen);

	free(buffer);
}
