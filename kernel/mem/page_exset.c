/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * page_exset
 *
 * Sets the value of a page in an exmapped address space.
 */

void page_exset(uintptr_t seg, uintptr_t page, frame_t value) {
	frame_t *exmap, *extbl;

	extbl = (void*) seg;
	exmap = (void*) (seg + 0x3FF000);

	if ((exmap[page >> 22] & PF_PRES) == 0) {
		page_extouch(seg, page);
	}

	extbl[page >> 12] = value;
}
