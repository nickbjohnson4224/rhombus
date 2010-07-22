/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>

char *strrchr(const char *s, char c) {
	size_t i;

	for (i = 0; s[i]; i++);

	for (; i; i--) {
		if (s[i] == c) {
			return (char*) &s[i];
		}
	}

	return NULL;
}
