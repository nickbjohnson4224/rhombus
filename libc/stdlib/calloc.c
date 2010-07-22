/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

void *calloc(size_t nmemb, size_t size) {
	void *ptr;

	ptr = malloc(size * nmemb);

	if (!ptr) {
		return NULL;
	}

	memclr(ptr, size * nmemb);
	
	return ptr;
}
