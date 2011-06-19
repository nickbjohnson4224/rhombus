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
 * fb_flip
 *
 * Synchronizes the contents of the framebuffer with the framebuffer
 * device. Returns zero on success, nonzero on error.
 */

int fb_flip(struct fb *fb) {
	char buf[40];
	char *ret;

	if (!fb) {
		return 1;
	}

	mutex_spin(&fb->mutex);

	// check if flipping is useless
	if (fb->minx > fb->maxx || fb->miny > fb->maxy) {
		mutex_free(&fb->mutex);
		return 0;
	}

	if (fb->flags & FB_SHARED) {
		// shared: just sync
		sprintf(buf, "syncrect %d %d %d %d", fb->minx, fb->miny, 
			fb->maxx - fb->minx, fb->maxy - fb->miny);
		ret = rcall(fb->rp, buf);
		if (!ret || !strcmp(ret, "")) {
			mutex_free(&fb->mutex);
			return 1;
		}
		free(ret);
	}
	else {
		// not shared: write whole buffer
		if (!write(fb->rp, fb->bitmap, 
				fb->xdim * fb->ydim * sizeof(uint32_t), 0)) {
			mutex_free(&fb->mutex);
			return 1;
		}
	}
	
	// reset flip acceleration
	fb->minx = fb->xdim + 1;
	fb->miny = fb->ydim + 1;
	fb->maxx = 0;
	fb->maxy = 0;

	mutex_free(&fb->mutex);
	return 0;
}
