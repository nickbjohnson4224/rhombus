/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <string.h>

int memcmp(const void *p1, const void *p2, size_t n) {
	#ifndef arch_memcmp
		uint8_t *v1 = (void*) p1;
		uint8_t *v2 = (void*) p2;

		while (n--) {
			if (*v1++ != *v2++) {
				return 0;
			}
		}

		return 1;
	#else
		return arch_memcmp(p1, p2, n);
	#endif
}
