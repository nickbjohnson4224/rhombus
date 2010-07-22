/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch.h>

void *valloc(size_t size) {
	return aalloc(size, PAGESZ);
}
