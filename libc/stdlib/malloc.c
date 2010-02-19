/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/heap.h>

#include <stdint.h>
#include <stdlib.h>

void *malloc(size_t size) {
	return heap_malloc(size);
}
