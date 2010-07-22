/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>

int strcmp(const char *s1, const char *s2) {
	size_t i;

	for (i = 0;; i++) {
		if (s1[i] == s2[i]) {
			if (s1[i] == '\0') return 0;
			continue;
		}
		if (s1[i] == '\0') return -1;
		if (s2[i] == '\0') return 1;
		if (s1[i] < s2[i]) return -1;
		else return 1;
	}
}
