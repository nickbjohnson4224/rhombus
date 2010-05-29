/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <string.h>

size_t strlcpy(char *dest, const char *src, size_t n) {
	#ifndef arch_strlcpy
		size_t i;

		if (n) {
			for (i = 0; i < n - 1; i++) {
				dest[i] = src[i];
				if (!dest[i]) {
					break;
				}
			}

			if (i == n - 1) {
				dest[i] = '\0';
			}
		}

		return strlen(src);
	#else
		return arch_strlcpy(dest, src, n);
	#endif
}
