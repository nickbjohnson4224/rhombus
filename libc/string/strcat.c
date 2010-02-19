/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>

char *strcat(char *d, const char *s) {
	size_t i, e;

	for (e = 0; d[e] != '\0'; e++);

	for (i = 0; s[i] != '\0'; i++) {
		d[e + i] = s[i];
	}

	d[e + i] = '\0';

	return d;
}
