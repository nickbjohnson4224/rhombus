/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

frame_t *cmap = (void*) (PGE_MAP + 0x3FF000);	/* Current page directory mapping */
frame_t *ctbl = (void*) PGE_MAP;				/* Current base page table mapping */

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

/****************************************************************************
 * page_touch
 *
 * Ensures that the segment containing a page exists.
 */

void page_touch(uintptr_t page) {
	page_extouch(PGE_MAP, page);
}
