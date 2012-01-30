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
#include <rho/page.h>

/****************************************************************************
 * fb_cons
 *
 * Set up a framebuffer interface for the resource <rp>. Returns NULL on
 * error.
 */

struct fb *fb_cons(uint64_t rp) {
	struct fb *fb;
	char *mode;

	if (!rp) {
		return NULL;
	}

	// allocate and setup framebuffer
	fb = malloc(sizeof(struct fb));
	fb->fd    = ropen(-1, rp, STAT_WRITER);
	fb->mutex = false;
	fb->flags = 0;
	
	// check video mode
	mode = rcall(rp, 0, "getmode");

	if (!mode) {
		close(fb->fd);
		free(fb);
		return NULL;
	}

	sscanf(mode, "%i %i", &fb->xdim, &fb->ydim);
	free(mode);

	// allocate bitmap
	fb->bitmap = aalloc(sizeof(uint32_t) * fb->xdim * fb->ydim, PAGESZ);
	memclr(fb->bitmap, sizeof(uint32_t) * fb->xdim * fb->ydim);

	// check for shared memory interface
	if (rp_mmap(rp, fb->bitmap, 
			fb->xdim * fb->ydim * sizeof(uint32_t), 0, PROT_READ | PROT_WRITE)) {
		// successful
		fb->flags |= FB_SHARED;
	}

	// set up flip acceleration
	fb->minx = fb->xdim + 1;
	fb->miny = fb->ydim + 1;
	fb->maxx = 0;
	fb->maxy = 0;

	return fb;
}

struct fb *fb_createwindow() {
	uint64_t wmanager;
	char *ret;
	
	wmanager = fs_find("/sys/wmanager/");
	if (!wmanager) {
		return NULL;
	}
	
	ret = rcall(wmanager, 0, "createwindow");
	return fb_cons(ator(ret));
}
