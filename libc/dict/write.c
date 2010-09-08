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
 * dwrite_rec
 *
 * Recursive dictionary writing function, used by interface write functions.
 * Sets the dictionary value to the given value unless the given value is
 * null, in which case the value is deleted. This function is not 
 * thread-safe.
 */

static int dwrite_rec(struct __dict *root, const char *value, const char *key) {

	if (root->link) {
		return _dlink_write(root->link, value, key);
	}

	if (!key[0]) {
		if (value) {
			root->value = dalloc(strlen(value) + 1);
			strcpy(root->value, value);
		}
		else {
			root->value = NULL;
		}
		return 0;
	}
	
	if (!root->next[(size_t) key[0]]) {
		root->next[(size_t) key[0]] = dalloc(sizeof(struct __dict));
	}

	return dwrite_rec(root->next[(size_t) key[0]], value, &key[1]);
}

/****************************************************************************
 * dwrite
 *
 * Write value to dictionary with the given key pair. Returns 0 on success,
 * nonzero on failure. This function is thread-safe.
 */

int dwrite(const char *value, const char *key) {
	int err;

	mutex_spin(&(dict_info->mutex));
	err = dwrite_rec(&dict_info->root, value, key);
	mutex_free(&(dict_info->mutex));

	return err;
}

/****************************************************************************
 * dwritens
 *
 * Read from the dictionary from a given namespace using the given string as
 * a key. Returns 0 on success, nonzero on failure. This function is 
 * thread-safe.
 */

int dwritens(const char *value, const char *ns, const char *key) {
	char *buffer;
	int err;

	buffer = malloc(strlen(ns) + strlen(key) + 1);
	strcpy(buffer, ns);
	strcat(buffer, key);

	err = dwrite(value, buffer);

	free(buffer);

	return err;
}
