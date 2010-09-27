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
#include <natio.h>

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
		return dwriter(root->link, value, key);
	}

	if (!key[0]) {
		if (value) {
			root->value = strdup(value);
		}
		else {
			root->value = NULL;
		}
		return 0;
	}
	
	if (!root->next[(size_t) key[0]]) {
		root->next[(size_t) key[0]] = calloc(sizeof(struct __dict), 1);
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

	mutex_spin(&dict_mutex);
	err = dwrite_rec(dict_root, value, key);
	mutex_free(&dict_mutex);

	return err;
}

/****************************************************************************
 * dwriter
 *
 * XXX - doc
 */

int dwriter(FILE *remote, const char *value, const char *key) {
	struct __link_req req;
	size_t size;

	strlcpy(req.key, key, 2048);
	strlcpy(req.val, value, 2048);

	size = ssend(remote, &req, &req, sizeof(struct __link_req), 0, PORT_DREAD);

	if (!size) {
		return 1;
	}
	else {
		return 0;
	}
}
