/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>

char *strstr(const char *s1, const char *s2) {
	size_t i, j;

	for (i = 0; s1[i]; i++) {
		for (j = 0; s2[j] && s1[i+j]; j++) {
			if (s1[i+j] != s2[j]) {
				break;
			}
		}
		if (s2[j] == '\0') {
			return (char*) &s1[i];
		}
	}

	return NULL;
}
