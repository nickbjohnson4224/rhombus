/* Copyright 2010 Nick Johnson */

#include <flux/heap.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void *realloc(void *ptr, size_t size) {
	void *new;
	
	new = heap_malloc(size);
	memcpy(new, ptr, heap_size(ptr));
	heap_free(ptr);

	return new;
}
