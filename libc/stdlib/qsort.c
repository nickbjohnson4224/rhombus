/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
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
 * qsort
 *
 * Sorts the array <base> containing <nmemb> elements of size <size> in
 * ascending order based on <cmp>.
 */

void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void*)) {
	if (nmemb <= 1) {
		return;
	}

	size_t left = 0;
	size_t right = nmemb - 1;
	void *pivot = malloc(size);
	void *t = malloc(size);
	memcpy(pivot, (uint8_t*) base + (left + right) / 2 * size, size);

	while (1) {
		// Find items to swap
		while (left < nmemb && compar((uint8_t*) base + left * size, pivot) < 0) {
			left++;
		}
		while (right > 0 && compar((uint8_t*) base + right * size, pivot) > 0) {
			right--;
		}

		if (left >= right) {
			// all items are divided
			break;
		}
		if (compar((uint8_t*) base + left * size, (uint8_t*) base + right * size) == 0) {
			// skip equal items
			left++;
			continue;
		}

		// ... and swap them
		memcpy(t, (uint8_t*) base + right * size, size);
		memcpy((uint8_t*) base + right * size, (uint8_t*) base + left * size, size);
		memcpy((uint8_t*) base + left * size, t, size);
	}

	free(pivot);
	free(t);

	// sort items less than pivot and greater than pivot recursively
	qsort(base, left, size, compar);
	qsort((uint8_t*) base + (left + 1) * size, nmemb - left - 1, size, compar);
}
