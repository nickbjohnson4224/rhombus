/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * page_extouch
 *
 * Makes sure that a page in an exmapped address space exists.
 */

void page_extouch(uintptr_t seg, uintptr_t page) {
	frame_t *exmap, *extbl;

	extbl = (void*) seg;
	exmap = (void*) (seg + 0x3FF000);
	page &= ~0x3FFFFF;

	if (exmap[page >> 22] & PF_PRES) {
		return;
	}

	exmap[page >> 22]  = frame_new() | PF_PRES | PF_RW | PF_USER;
	exmap[page >> 22] |= SEG_USED | SEG_ALLC;

	if (page > KSPACE) {
		exmap[page >> 22] |= SEG_LINK;
	}
	
	page_flush((uintptr_t) &extbl[page >> 12]);

	pgclr(&extbl[page >> 12]);
}
