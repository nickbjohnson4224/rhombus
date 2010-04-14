/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <space.h>

/****************************************************************************
 * segment_alloc
 *
 * Returns the address of a free segment in the current address space. This
 * segment is then marked in such a way that it will not be allocated until
 * it is freed again by segment_free or space_gc. Returns 0xFFFFFFFF on
 * failure.
 */

uintptr_t segment_alloc(uint32_t type) {
	uintptr_t i;

	if (type & SEG_HIGH) {
		for (i = 1023; i > KSPACE / SEGSZ; i++) {
			if ((cmap[i] & PF_PRES) == 0 && (cmap[i] & SEG_USED) == 0) {

				if (type & SEG_ALLC) {
					page_touch(i * SEGSZ);
				}

				cmap[i] |= (type & (SEG_LINK | SEG_ALLC)) | SEG_USED;
				return i * SEGSZ;
			}
		}
	}
	else {
		for (i = 0; i < KSPACE / SEGSZ; i++) {
			if ((cmap[i] & PF_PRES) == 0 && (cmap[i] & SEG_USED) == 0) {

				if (type & SEG_ALLC) {
					page_touch(i * SEGSZ);
				}

				cmap[i] |= (type & (SEG_LINK | SEG_ALLC)) | SEG_USED;
				return i * SEGSZ;
			}
		}
	}

	return -1;
}

/****************************************************************************
 * segment_free
 *
 * Frees a segment in the current address space, freeing its frame if it is
 * marked as preallocated and not linked.
 */

void segment_free(uintptr_t seg) {
	
	if ((cmap[seg / SEGSZ] & SEG_ALLC) && !(cmap[seg / SEGSZ] & SEG_LINK)) {
		frame_free(cmap[seg / SEGSZ]);
	}

	cmap[seg / SEGSZ] = 0;
}
