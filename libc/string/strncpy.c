/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <string.h>

char *strncpy(char *d, const char *s, size_t n) {
	#ifndef arch_strncpy
		size_t i;

		for (i = 0; s[i] != '\0' && i < n; i++) {
			d[i] = s[i];
		}

		return d;
	#else
		return arch_strncpy(d, s, n);
	#endif
}
