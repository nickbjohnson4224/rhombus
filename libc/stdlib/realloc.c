/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void *realloc(void *ptr, size_t size) {
	void *new;
	
	new = malloc(size);
	memcpy(new, ptr, msize(ptr));
	free(ptr);

	return new;
}
