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

#include <stdlib.h>

#include <rho/graph.h>
#include <rho/mutex.h>
#include <rho/natio.h>
#include <rho/page.h>

/*****************************************************************************
 * fb_setbmp
 *
 * Replaces the framebuffer internal state with <bitmap>. The given pointer
 * must point to a memory region of fb->xdim * fb->ydim pixels and be PAGESZ
 * aligned, or there will be problems. If the previous internal state was set 
 * by fb_setbmp, it is not freed; otherwise, it is freed. Returns zero on 
 * success, nonzero on error.
 */

int fb_setbmp(struct fb *fb, uint32_t *bitmap) {

	if (!fb) {
		return 1;
	}

	mutex_spin(&fb->mutex);

	// unshare old bitmap
	if (fb->flags & FB_SHARED) {
		rp_share(fb->rp, NULL, 0, 0, 0);
		fb->flags &= ~FB_SHARED;
	}

	// free old bitmap
	if (fb->bitmap && !(fb->flags & FB_USRBMP)) {
		free(fb->bitmap);
	}

	// set bitmap
	fb->bitmap = bitmap;
	fb->flags |= FB_USRBMP;

	// check for shared memory interface
	if (!rp_share(fb->rp, fb->bitmap, 
			fb->xdim * fb->ydim * sizeof(uint32_t), 0, PROT_READ)) {
		// successful
		fb->flags |= FB_SHARED;
	}

	mutex_free(&fb->mutex);

	return 0;
}
