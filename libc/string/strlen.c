/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <string.h>

size_t strlen(const char *s) {
	size_t i;

	for (i = 0; s[i]; i++);

	return i;
}
