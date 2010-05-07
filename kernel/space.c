/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <space.h>

/****************************************************************************
 * space_alloc
 *
 * Returns a new address space.
 */

space_t space_alloc(void) {
	space_t space = frame_new();
	uint32_t *map = (void*) TMP_SRC;

	/* clear new space */
	page_set(TMP_SRC, page_fmt(space, PF_PRES | PF_RW));
	pgclr(map);

	/* set recursive mapping */
	map[PGE_MAP >> 22] = page_fmt(space, PF_PRES | PF_RW | SEG_USED);

	return space;
}

/****************************************************************************
 * space_clone
 *
 * Copies the currently loaded address space so that the new address space 
 * is independent of the old one.
 */

static void segment_clone(frame_t *extbl, frame_t *exmap, uintptr_t seg);

space_t space_clone() {
	uint32_t i;
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

/* helper for space_clone */
static void segment_clone(frame_t *extbl, frame_t *exmap, uintptr_t seg) {
	uintptr_t i;

	exmap[seg / SEGSZ] = frame_new() | PF_PRES | PF_USER | PF_RW;
	pgclr(&extbl[seg / PAGESZ]);

	for (i = seg / PAGESZ; i < (seg + SEGSZ) / PAGESZ; i++) {
		if (ctbl[i] & PF_PRES) {
			extbl[i] = frame_new() | (ctbl[i] & PF_MASK);
			page_flush_full();
			page_set(TMP_SRC, page_fmt( ctbl[i], PF_PRES | PF_RW));
			page_set(TMP_DST, page_fmt(extbl[i], PF_PRES | PF_RW));
			memcpy((void*) TMP_DST, (void*) TMP_SRC, PAGESZ);
		}
	}
}

/****************************************************************************
 * space_exmap
 *
 * Recursively maps an external address space in the given segment.
 */

void space_exmap(uintptr_t seg, space_t space) {
	cmap[seg >> 22] = page_fmt(space, PF_PRES | PF_RW | SEG_LINK | SEG_USED);
	page_flush_full();
}

/****************************************************************************
 * space_free
 *
 * Completely frees resources associated with an address space.
 */

void space_free(space_t space) {
	uint32_t i, j;
	uintptr_t seg;
	frame_t *extbl, *exmap;

	seg = TMP_MAP;

	space_exmap(seg, space);
	extbl = (void*) seg;
	exmap = (void*) (seg + 0x3FF000);

	for (i = 0; i < PGE_MAP / SEGSZ; i++) {
		if ((exmap[i] & PF_PRES) && !(exmap[i] & SEG_LINK)) {
			for (j = 0; j < 1024; j++) {
				if (extbl[i * 1024 + j] & PF_PRES) {
					frame_free(page_ufmt(extbl[i * 1024 + j]));
				}
				extbl[i * 1024 + j] = 0;
			}
			frame_free(page_ufmt(exmap[i]));
			exmap[i] = 0;
		}
	}

	frame_free(space);
}
