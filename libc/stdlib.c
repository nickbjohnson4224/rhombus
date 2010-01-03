/* Copyright 2009 Nick Johnson */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include "libc.h"

void *calloc(size_t nmemb, size_t size) {
	void *ptr;
	
	ptr = _heap_alloc(nmemb * size);
	memclr(ptr, nmemb * size);
	
	return ptr;
}

void *malloc(size_t size) {
	void *ptr;

/*	sigblock(); */
	ptr = _heap_alloc(size);
/*	sigunblock(); */

	return ptr;
}

void free(void *ptr) {
/*	sigblock(); */
	_heap_free(ptr);
/*	sigunblock(); */
}

void *realloc(void *ptr, size_t size) {
	void *new;

/*	sigblock(); */
	new = _heap_alloc(size);
	memcpy(new, ptr, _heap_size(ptr));
	_heap_free(ptr);
/*	sigunblock(); */

	return new;
}
