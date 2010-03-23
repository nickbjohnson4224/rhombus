/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

static void segment_clone(frame_t *extbl, frame_t *exmap, uintptr_t seg);

/****************************************************************************
 * space_clone
 *
 * Copies the currently loaded address space so that the new address space 
 * is independent of the old one.
 */

space_t space_clone() {
	uint32_t i, j;
	uintptr_t seg;
	space_t dest;
	frame_t *exmap, *extbl;

	/* Create new address space */
	dest = space_alloc();

	/* Exmap in new address space */
	seg = TMP_MAP;
	space_exmap(seg, dest);
	extbl = (void*) seg;
	exmap = (void*) (seg + 0x3FF000);

	/* Clone/clear userspace */
	for (i = 0; i < 1023; i++) {
		if (cmap[i] & PF_PRES) {
			if (cmap[i] & SEG_LINK) {
				exmap[i] = cmap[i];
			}
			else {
				segment_clone(extbl, exmap, i * SEGSZ);
			}
		}
	}

	return dest;
}

static void segment_clone(frame_t *extbl, frame_t *exmap, uintptr_t seg) {
	uintptr_t i;

	exmap[seg / SEGSZ] = frame_new() | PF_PRES | PF_USER | PF_RW;
	pgclr(&extbl[seg / PAGESZ]);

	for (i = seg / PAGESZ; i < (seg + SEGSZ) / PAGESZ; i++) {
		if (ctbl[i] & PF_PRES) {
			extbl[i] = frame_new() | (ctbl[i] & PF_MASK);
			page_flush_full();
			page_set((uint32_t) tsrc, page_fmt( ctbl[i], PF_PRES | PF_RW));
			page_set((uint32_t) tdst, page_fmt(extbl[i], PF_PRES | PF_RW));
			memcpy(tdst, tsrc, PAGESZ);
		}
	}
}
