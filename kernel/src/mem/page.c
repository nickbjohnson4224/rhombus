// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

pool_t fpool[1024];
ptbl_t *cmap = (void*) 0xFFFFF000;
page_t *ctbl = (void*) 0xFFC00000;

void page_touch(u32int page) {
	if (cmap[page >> 22] & PF_PRES) return;
	cmap[page >> 22] = frame_new() | (PF_PRES | PF_RW);
	pgclr(&ctbl[(page &~ 0xFFF) >> 12]);
}

void page_set(u32int page, page_t value) {
	if ((cmap[page >> 22] & PF_PRES) == 0) page_touch(page);
	ctbl[page >> 12] = value;
	asm volatile ("invlpg %0" :: "m" (page));
}

page_t page_get(u32int page) {
	return ((cmap[page >> 22] & 0x1) == 0) ? 0 : ctbl[page >> 12];
}
