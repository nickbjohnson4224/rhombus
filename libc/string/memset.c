/* Copyright 2010 Nick Johnson */

#include <string.h>
#include <stdint.h>

void *memset(void *s, uint8_t c, size_t n) {
	size_t i;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		src[i] = c;
	}

	return src;
}
