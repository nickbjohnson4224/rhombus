/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <dict.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>

void *dict_alloc(size_t size, bool data) {
	void *ptr;

	ptr = (void*) dict_info->brk;
	dict_info->brk += size;

	mmap(ptr, size + PAGESZ, MMAP_READ | MMAP_WRITE);

	memclr(ptr, size);
	return ptr;
}

void dict_sweep(void) {
	return;
}
