/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <hash.h>

bool _has_init = false;
uint8_t _table[256];
uint8_t _table_ext[4][256];

void _hash_pearson_init() {
	size_t i;
	size_t j;
	uint8_t k;
	uint8_t s;
	uint8_t r;
	
	// fill _table with a pseuodrandom permutation of 0..255.
	for (i = 0; i < 255; i++) {
		_table[i] = i;
	}

	k = 7;
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 255; i++) {
			k += _table[i];
			s = _table[i];
			_table[i] = _table[k];
			_table[k] = s;
		}
	}

	// fill _table_ext with pseuodrandom permutations of 0..255.
	for (k = 0; k < 8; k++) {
		for (i = 0; i < 255; i++) {
			_table_ext[k][i] = i;
		}

		r = 7 + 4 * k;
		for (j = 0; j < 3; j++) {
			for (i = 0; i < 255; i++) {
				r += _table_ext[k][i];

				s = _table_ext[k][i];
				_table_ext[k][i] = _table_ext[k][r];
				_table_ext[k][r] = s;
			}
		}
	}

	_has_init = true;
}

uint8_t hash_pearson8(const void *data, size_t length) {
	const uint8_t *data8 = data;
	uint8_t hash;
	size_t i;

	if (!_has_init) _hash_pearson_init();

	hash = 0;
	for (i = 0; i < length; i++) {
		hash ^= data8[i];
		hash = _table[hash];
	}

	return hash;
}

uint8_t hash_pearson8_str(const char *str) {
	const uint8_t *str8 = (const uint8_t*) str;
	uint8_t hash;

	if (!_has_init) _hash_pearson_init();

	hash = 0;
	while (*str8) {
		hash ^= *str8;
		hash = _table[hash];
		str8++;
	}

	return hash;
}
