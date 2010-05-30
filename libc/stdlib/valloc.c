/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/heap.h>

#include <stdint.h>
#include <stdlib.h>

void *valloc(size_t size) {
	return heap_valloc(size);
}
