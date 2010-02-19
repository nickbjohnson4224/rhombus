/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <mem.h>

/* Simple stack-based virtual memory allocator for the init system */

#ifdef KERNEL_GC
__attribute__ ((section(".idata"))) 
#endif
uint32_t allocator_space = KMALLOC;

#ifdef KERNEL_GC
__attribute__ ((section(".itext"))) 
#endif
void *kmalloc(uint32_t size) {
	allocator_space += size;

	return (void*) (allocator_space - size);
}
