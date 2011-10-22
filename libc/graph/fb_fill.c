/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <rho/natio.h>
#include <rho/graph.h>
#include <rho/mutex.h>

/****************************************************************************
 * fb_fill
 *
 * Fill region with coordinates (<x>, <y>) and dimensions <w> by <h>,
 * applying alpha effects. Returns zero on success, nonzero on error.
 */

int fb_fill(struct fb *fb, int x, int y, int w, int h, uint32_t color) {
	int i, j;
	
	if (!fb) {
		return 1;
	}

	// bounds check
	if (x < 0 || y < 0 || y + h > fb->ydim || x + w > fb->xdim) {
		return 1;
	}

	mutex_spin(&fb->mutex);

	// set pixel value
	for (i = y; i < y + h && i < fb->ydim; i++) {
		for (j = x; j < x + w && j < fb->xdim; j++) {
			fb->bitmap[j + i * fb->xdim] = color;
		}
	}

	// update flip acceleration
	if (fb->minx >  x)     fb->minx = x;
	if (fb->maxx <= x + w) fb->maxx = x + w+ 1;
	if (fb->miny >  y)     fb->miny = y;
	if (fb->maxy <= y + h) fb->maxy = y + h + 1;

	mutex_free(&fb->mutex);

	return 0;
}
