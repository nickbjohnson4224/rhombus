/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>

char *strcpy(char *d, const char *s) {
	size_t i;

	for (i = 0; s[i] != '\0'; i++) {
		d[i] = s[i];
	}
	
	d[i] = '\0';

	return d;
}
