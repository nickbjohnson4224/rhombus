// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

/* 	This allocator can only allocate single pages
 *	If I build the kernel right, this will suffice  */

extern u32int end;
u32int proto_base = 0xF8010000; // Use up the lower memory for allocation if possible

void *kmalloc(u32int size) {
	u32int i, addr, temp;

	if (!mem_setup) {
		if (proto_base & 0xFFF) proto_base = (proto_base & ~0xFFF) + 0x1000;
		temp = proto_base;
		proto_base += size;
		if (proto_base >= 0xF8080000) {
			proto_base = (u32int) &end; // Don't overwrite EBDA!
			panic("initial allocator overrun\n");
		}
		return (void*) temp;
	}
	else {
		addr = (pool_alloc(ppool) << 12) + 0xF8400000;
		page_set(&kmap, addr, page_fmt(frame_new(), PF_PRES | PF_RW));
		return (void*) addr;
	}
}

void kfree(void *addr) {
	pool_free(ppool, ((u32int) addr - 0xF8400000) >> 12);
}

u32int frame_new() {
	return (pool_alloc(fpool) << 12);
}

void frame_free(u32int addr) {
	pool_free(fpool, addr >> 12);
}
