/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * space_exmap
 *
 * Recursively maps an external address space in the given segment.
 */

void space_exmap(uintptr_t seg, space_t space) {
	cmap[seg >> 22] = page_fmt(space, PF_PRES | PF_RW | SEG_LINK | SEG_USED);
	page_flush_full();
}
