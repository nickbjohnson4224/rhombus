/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

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
