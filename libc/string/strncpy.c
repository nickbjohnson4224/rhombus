/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>

char *strncpy(char *d, const char *s, size_t n) {
	size_t i;

	for (i = 0; s[i] != '\0' && i < n; i++) {
		d[i] = s[i];
	}

	return d;
}
