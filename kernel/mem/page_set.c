/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * page_set
 *
 * Sets a page in the current address space to a value.
 */

void page_set(uintptr_t page, frame_t value) {

	if ((cmap[page >> 22] & PF_PRES) == 0) {
		page_touch(page);
	}

	ctbl[page >> 12] = value;
	page_flush(page);
}
