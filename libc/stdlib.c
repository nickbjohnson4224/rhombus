/* Copyright 2009 Nick Johnson */

#include <flux.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <_libc.h>

void *calloc(size_t nmemb, size_t size) {
	void *ptr;
	
	ptr = _heap_alloc(nmemb * size);
	memclr(ptr, nmemb * size);
	
	return ptr;
}

void *malloc(size_t size) {
	void *ptr;

	sigblock(true);

	ptr = _heap_alloc(size);

	sigblock(false);

	return ptr;
}

void free(void *ptr) {

	sigblock(true);

	_heap_free(ptr);

	sigblock(false);
}

void *realloc(void *ptr, size_t size) {
	void *new;

	sigblock(true);
	
	new = _heap_alloc(size);
	memcpy(new, ptr, _heap_size(ptr));
	_heap_free(ptr);

	sigblock(false);

	return new;
}

int atoi(const char *nptr) {
	int i, sum;

	for (sum = 0, i = strlen(nptr) - 1; i >= 0; i--) {
		sum *= 10;
		sum += nptr[i] - '0';
	}

	return sum;
}
