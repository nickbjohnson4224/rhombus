/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <string.h>

char *strncat(char *d, const char *s, size_t n) {
	size_t i, e;

	for (e = 0; d[e] != '\0'; e++);

	for (i = 0; s[i] != '\0' && i < n; i++) {
		d[e + i] = s[i];
	}

	d[e + i] = '\0';

	return d;
}
