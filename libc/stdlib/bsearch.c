/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

/****************************************************************************
 * bsearch
 *
 * Finds the element in the array <base>, with <nmemb> elements of size <size>
 * sorted in ascending order based on <cmp>, that matches <key>.
 */

const void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
		int (*cmp)(const void *keyval, const void *datum)) {
	size_t lower, upper;
	const uint8_t *array;
	int result;

	lower = 0;
	upper = nmemb;
	array = base;

	while (1) {
		result = cmp(key, &array[((lower + upper) / 2) * size]);

		if (result < 0) {
			/* guess was too low */
			upper = (lower + upper) / 2;
		}
		else if (result > 0) {
			/* guess was too high */
			lower = 1 + (lower + upper) / 2;
		}
		else {
			/* found match */
			return &array[((lower + upper) / 2) * size];
		}
	}
}
