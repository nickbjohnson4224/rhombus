/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * page_get
 *
 * Returns the value of a page in the current address space.
 */

frame_t page_get(uintptr_t page) {

	if ((cmap[page / SEGSZ] & PF_PRES) == 0) {
		return 0;
	}

	return ctbl[page / PAGESZ];
}
