/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/heap.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void *calloc(size_t nmemb, size_t size) {
	void *ptr;
	
	ptr = heap_malloc(nmemb * size);

	if (!ptr) {
		return NULL;
	}

	memclr(ptr, nmemb * size);
	
	return ptr;
}
