/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>

void *memccpy(void *d, const void *s, uint8_t c, size_t n) {
	size_t i;
	uint8_t *dst = (uint8_t*) d; 
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
		if (src[i] == c) {
			return &dst[i+1];
		}
	}

	return NULL;
}
