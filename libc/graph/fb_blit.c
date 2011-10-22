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
#include <stdio.h>

#include <rho/natio.h>
#include <rho/graph.h>
#include <rho/mutex.h>

/*****************************************************************************
 * fb_blit
 *
 * Copy the bitmap <bitmap> (with dimensions <w> by <h>) to the frambuffer
 * at starting coordinates (<x>, <y>), and apply alpha effects. Returns zero
 * on success, nonzero on error.
 */

int fb_blit(struct fb *fb, uint32_t *bitmap, int x, int y, int w, int h) {
	int i, j;
	int x0, y0, x1, y1;
	
	if (!fb) {
		return 1;
	}

	mutex_spin(&fb->mutex);

	// copy data
	i = (x < 0) ? 0 : x;
	for (; i < x + w && i < fb->xdim; i++) {
		j = (y < 0) ? 0 : y;
		for (; j < y + h && j < fb->ydim; j++) {
			fb->bitmap[i + j * fb->xdim] = bitmap[i - x + (j - y) * w];
		}
	}

	// update flip acceleration
	x0 = (x < 0) ? 0 : x;
	y0 = (y < 0) ? 0 : y;
	x1 = (x + w > fb->xdim - 1) ? fb->xdim - 1 : x + w;
	y1 = (y + h > fb->ydim - 1) ? fb->ydim - 1 : y + h;
	if (fb->minx > x0) fb->minx = x0;
	if (fb->maxx < x1) fb->maxx = x1;
	if (fb->miny > y0) fb->miny = y0;
	if (fb->maxy < y1) fb->maxy = y1;

	mutex_free(&fb->mutex);

	return 0;
}
