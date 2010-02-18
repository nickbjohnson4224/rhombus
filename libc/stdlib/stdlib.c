/* Copyright 2009, 2010 Nick Johnson */

#include <flux/heap.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void *calloc(size_t nmemb, size_t size) {
	void *ptr;
	
	ptr = heap_malloc(nmemb * size);
	memclr(ptr, nmemb * size);
	
	return ptr;
}

void *malloc(size_t size) {
	return heap_malloc(size);
}

void free(void *ptr) {
	heap_free(ptr);
}

void *realloc(void *ptr, size_t size) {
	void *new;
	
	new = heap_malloc(size);
	memcpy(new, ptr, heap_size(ptr));
	heap_free(ptr);

	return new;
}

int atoi(const char *nptr) {
	int i, sum;

	for (sum = 0, i = 0; nptr[i]; i++) {
		sum *= 10;
		sum += nptr[i] - '0';
	}

	return sum;
}
