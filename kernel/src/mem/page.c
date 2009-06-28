// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

pool_t fpool[1024];

void page_touch(u32int page) {
	page &= ~0x3FFFFF;
	if (cmap[page >> 22] & PF_PRES) return;
//	printk("page_touch %x", page);
	cmap[page >> 22] = frame_new() | (PF_PRES | PF_RW);
	pgclr(&ctbl[page >> 12]);
}

void page_set(u32int page, page_t value) {
	if ((cmap[page >> 22] & PF_PRES) == 0) page_touch(page);
	ctbl[page >> 12] = value;
	asm volatile ("invlpg %0" :: "m" (page));
}

page_t page_get(u32int page) {
	return (cmap[page >> 22] & PF_PRES) ? ctbl[page >> 12] : 0;
}
