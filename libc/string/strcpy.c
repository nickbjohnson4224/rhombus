/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>

#include <string.h>
#include <stdint.h>

char *strcpy(char *d, const char *s) {
	#ifndef arch_strcpy
		size_t i;

		for (i = 0; s[i] != '\0'; i++) {
			d[i] = s[i];
		}
	
		d[i] = '\0';

		return d;
	#else
		return arch_strcpy(d, s);
	#endif
}
