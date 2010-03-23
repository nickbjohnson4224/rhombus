/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * page_exget
 *
 * Returns the value of a page in an exmapped address space
 */

frame_t page_exget(uintptr_t seg, uintptr_t page) {
	frame_t *exmap, *extbl;

	extbl = (void*) seg;
	exmap = (void*) (seg + 0x3FF000);

	if ((exmap[page >> 22] & PF_PRES) == 0) {
		return 0;
	}

	return extbl[page >> 12];
}
