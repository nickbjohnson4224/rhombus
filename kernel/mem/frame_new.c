/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * frame_new
 *
 * Returns the physcial address of an unused frame
 */

frame_t frame_new(void) {
	size_t i, j;

	for (i = 0; i < nframe_bitmap; i++) {
		if (frame_bitmap[i] == 0xFFFFFFFF) continue;
		for (j = 0; j < 32; j++) {
			if ((frame_bitmap[i] & (1 << j)) == 0) {
				frame_bitmap[i] |= (1 << j);
				return PAGESZ * (i * 32 + j);
			}
		}
	}

	return -1;
}
