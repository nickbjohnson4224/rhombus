/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

uint32_t vframe_bitmap[32768]; /* space for bitmap */
uint32_t *frame_bitmap;        /* bitmap for allocating frames */
uint32_t nframe_bitmap;        /* size of bitmap in dwords */

/****************************************************************************
 * frame_free
 *
 * Frees a frame by physical address.
 */

void frame_free(frame_t frame) {
	frame /= PAGESZ;
	frame_bitmap[frame / 32] &= ~(1 << (frame % 32));
}

/****************************************************************************
 * frame_init
 *
 * Initializes the kernel frame allocator. After this function is run, the
 * functions frame_new and frame_free can be used. This function can be run
 * only once.
 */

void frame_init(uintptr_t memsize) {
	size_t i;

	frame_bitmap = &vframe_bitmap[0];
	nframe_bitmap = memsize / (PAGESZ * 32);

	/* clear bitmap */
	for (i = 0; i < nframe_bitmap; i++) {
		frame_bitmap[i] = 0;
	}

	/* set first 4 MB as used */
	for (i = 0; i < SEGSZ / (PAGESZ * 32); i++) {
		frame_bitmap[i] = -1;
	}
}

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
