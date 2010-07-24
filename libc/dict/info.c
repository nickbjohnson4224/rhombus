/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <dict.h>
#include <arch.h>
#include <mmap.h>
#include <string.h>

struct dict_info *dict_info = (void*) DICTIONARY;

void dict_init(void) {
	
	mmap(dict_info, sizeof(struct dict_info), MMAP_READ | MMAP_WRITE);

	memclr(dict_info, sizeof(struct dict_info));
	dict_info->brk = (uintptr_t) dict_info + sizeof(struct dict_info);
}
