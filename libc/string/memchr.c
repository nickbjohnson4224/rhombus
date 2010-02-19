/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>

void *memchr(const void *s, uint8_t c, size_t n) {
	size_t i;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		if (src[n] == c) return &src[n];
	}
	
	return NULL;
}
