/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * page_touch
 *
 * Ensures that the segment containing a page exists.
 */

void page_touch(uintptr_t page) {
	page_extouch(PGE_MAP, page);
/*	page &= ~0x3FFFFF;

	if (cmap[page >> 22] & PF_PRES) {
		return;
	}

	cmap[page >> 22]  = frame_new() | PF_PRES | PF_RW | PF_USER
	cmap[page >> 22] |= SEG_LINK | SEG_ALLC;

	if (page > KSPACE) {
		cmap[page >> 22] |= SEG_LINK;
	}

	page_flush((uintptr_t) &ctbl[page >> 12]);
	pgclr(&ctbl[page >> 12]); */
}
