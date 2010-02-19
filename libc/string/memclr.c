/* Copyright 2010 Nick Johnson */

#include <string.h>
#include <stdint.h>

void *memclr(void *s, size_t n) {
	size_t i;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		src[i] = 0;
	}

	return src;
}
