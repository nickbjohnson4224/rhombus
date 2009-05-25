// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

/* 	This allocator can only allocate single pages
 *	If I build the kernel right, this will suffice  */

void *kmalloc(u32int size) {
	u32int addr = (pool_alloc(ppool) << 12) + 0xF8400000;
	page_set(&kmap, addr, page_fmt(frame_new(), PF_PRES | PF_RW));
	return (void*) addr;
}

void kfree(void *addr) {
	pool_free(ppool, ((u32int) addr - 0xF8400000) >> 12);
}
