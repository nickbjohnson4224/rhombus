/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <natio.h>
#include <stdio.h>
#include <graph.h>
#include <mutex.h>

/****************************************************************************
 * fb_plot
 *
 * Plots a single pixel, applying alpha effects. Returns zero on success,
 * nonzero on error.
 */

int fb_plot(struct fb *fb, int x, int y, uint32_t color) {
	
	if (!fb) {
		return 1;
	}

	// bounds check
	if (x < 0 || y < 0 || y >= fb->ydim || x >= fb->xdim) {
		return 1;
	}

	mutex_spin(&fb->mutex);

	// set pixel value
	fb->bitmap[x + y * fb->xdim] = color;

	// update flip acceleration
	if (fb->minx >  x) fb->minx = x;
	if (fb->maxx <= x) fb->maxx = x + 1;
	if (fb->miny >  y) fb->miny = y;
	if (fb->maxy <= y) fb->maxy = y + 1;

	mutex_free(&fb->mutex);

	return 0;
}
