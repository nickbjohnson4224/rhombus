/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/heap.h>

#include <stdlib.h>

void free(void *ptr) {
	heap_free(ptr);
}
