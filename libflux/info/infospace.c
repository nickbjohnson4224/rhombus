/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/mmap.h>
#include <flux/info.h>

uintptr_t infobrk = 1;

size_t info_node_alloc(void) {
	uintptr_t n = infobrk;

	infobrk++;

	mmap(&infospace[n], sizeof(struct info_node), PROT_READ | PROT_WRITE);
	arch_memclr(&infospace[n], sizeof(struct info_node));

	return n;
}
