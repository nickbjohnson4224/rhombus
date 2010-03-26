/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * frame_free
 *
 * Frees a frame by physical address.
 */

void frame_free(frame_t frame) {
	frame /= PAGESZ;
	frame_bitmap[frame / 32] &= ~(1 << (frame % 32));
}
