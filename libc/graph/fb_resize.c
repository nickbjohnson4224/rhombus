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
#include <natio.h>
#include <graph.h>
#include <mutex.h>
#include <page.h>

/*****************************************************************************
 * fb_resize
 *
 * Resizes bitmap of the framebuffer <fb> to <xdim> by <ydim>.
 * Returns zero on success, nonzero on failure.
 */

int fb_resize(struct fb *fb, int xdim, int ydim) {
	if (!fb) {
		return 1;
	}

	if (xdim == fb->xdim && ydim == fb->ydim) {
		// nothing to do
		return 0;
	}

	if (fb->flags & FB_SHARED) {
		// release shared memory
		rp_share(fb->rp, NULL, 0, 0, 0);
	}

	// resize bitmap
	fb->xdim = xdim;
	fb->ydim = ydim;
	free(fb->bitmap);
	fb->bitmap = aalloc(sizeof(uint32_t) * xdim * ydim, PAGESZ);
	memclr(fb->bitmap, sizeof(uint32_t) * xdim * ydim);

	if (fb->flags & FB_SHARED) {
		// re-register shared memory
		rp_share(fb->rp, fb->bitmap, xdim * ydim * sizeof(uint32_t), 0, PROT_READ);
	}

	// reset flip acceleration
	fb->minx = fb->xdim + 1;
	fb->miny = fb->ydim + 1;
	fb->maxx = 0;
	fb->maxy = 0;

	return 0;
}
