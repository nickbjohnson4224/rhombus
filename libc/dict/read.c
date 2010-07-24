/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <dict.h>
#include <stdint.h>
#include <mutex.h>
#include <string.h>

/****************************************************************************
 * dict_read_rec
 *
 * Recursive dictionary reading function, used by interface read functions.
 * Returns null on failure, pointer to dictionary value on success. This
 * function is not thread-safe.
 */

static const uint8_t *dict_read_rec
		(struct dict *root, const uint8_t *key, size_t keylen, size_t keypos) {

	if (keypos >= keylen) {
		return root->value;
	}
	
	if (root->next[key[keypos]]) {
		return dict_read_rec(root->next[key[keypos]], key, keylen, keypos + 1);
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

const uint8_t *dict_read(const uint8_t *key, size_t keylen) {
	const uint8_t *value;

	mutex_spin(&dict_info->mutex);

	value = dict_read_rec(&dict_info->root, key, keylen, 0);

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

const uint8_t *dict_readstr(const char *key) {
	return dict_read((uint8_t*) key, strlen(key));
}
