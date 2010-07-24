/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <dict.h>
#include <stdint.h>
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

	if (keypos >= keylen) {
		if (val) {
			root->value = memcpy(dict_alloc(vallen, true), val, vallen);
		}
		else {
			root->value = NULL;
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

void dict_write
	(const uint8_t *key, size_t keylen, const uint8_t *val, size_t vallen) {

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

void dict_writestr(const char *key, const uint8_t *val, size_t vallen) {
	dict_write((uint8_t*) key, strlen(key), val, vallen);
}
