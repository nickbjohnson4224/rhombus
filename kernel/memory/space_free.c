/* 
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/memory.h>

/****************************************************************************
 * space_free
 *
 * Frees all possible resources allocated to an address space. This does not
 * include segments that have the SEGMENT_FLAG_COPY flag set. It also frees
 * the frames containing the paging structures, not just the pages.
 */

static void seg_clean(frame_t pseg);

space_t space_free(space_t space) {
	size_t i;
	void *mirror;

	mirror = temp_alloc();

	page_set(KSPACE, mirror, space);

	for (i = 0; i < PAGESZ / sizeof(frame_t); i++) {
		if (mirror[i] & FRAME_PRES) {
			if (mirror[i] & SEGMENT_FLAG_COPY) {
				mirror[i] = 0;
			} else {
				seg_clean(mirror[i]);
			}
		}
	}

	return 0;
}
