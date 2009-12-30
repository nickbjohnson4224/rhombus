/* Copyright 2009 Nick Johnson */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "libc.h"

void *calloc(size_t nmemb, size_t size) {
	void *ptr;
	
	ptr = _heap_alloc(nmemb * size);
	memclr(ptr, nmemb * size);
	
	return ptr;
}

void *malloc(size_t size) {
	return _heap_alloc(size);
}

void free(void *ptr) {
	_heap_free(ptr);
}

void *realloc(void *ptr, size_t size) {
	void *new;

	new = _heap_alloc(size);
	memcpy(new, ptr, _heap_size(ptr));
	_heap_free(ptr);

	return new;
}
