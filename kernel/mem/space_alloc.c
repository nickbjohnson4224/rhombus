/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * space_alloc
 *
 * Returns a new address space.
 */

space_t space_alloc(void) {
	space_t space = frame_new();

	/* clear new space */
	page_set((uintptr_t) tsrc, page_fmt(space, PF_PRES | PF_RW));
	pgclr(tsrc);

	/* set recursive mapping */
	tsrc[PGE_MAP >> 22] = page_fmt(space, PF_PRES | PF_RW | SEG_USED);

	return space;
}
