/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>

char *strchr(const char *s, char c) {
	size_t i;

	for (i = 0; s[i] != c; i++);

	return (char*) &s[i];
}
