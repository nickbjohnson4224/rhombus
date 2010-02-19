/* Copyright 2010 Nick Johnson */

#include <string.h>
#include <stdint.h>

void *memmove(void *d, const void *s, size_t n) {
	size_t i;
	uint8_t *dst = (uint8_t*) d;
	uint8_t *src = (uint8_t*) s;

	if (src == dst) return dst;
	if (src  > dst) {
		for (i = 0; i < n; i++) {
			dst[i] = src[i];
		}
	}
	if (src  < dst) {
		for (i = 0; i < n; i++) {
			dst[n - i - 1] = src[n - i - 1];
		}
	}

	return dst;
} 
