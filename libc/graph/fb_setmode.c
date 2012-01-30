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

#include <rho/natio.h>
#include <rho/graph.h>
#include <rho/mutex.h>
#include <rho/page.h>

/*****************************************************************************
 * fb_setmode
 *
 * Attempt to set the mode of the framebuffer <fb> to <xdim> by <ydim>.
 * Returns zero on success, nonzero on failure.
 */

int fb_setmode(struct fb *fb, int xdim, int ydim) {
	char args[40];
	char *ret;
	
	if (!fb) {
		return 1;
	}

	if (fb->flags & FB_SHARED) {
		// release shared memory
		rp_share(fb->rp, NULL, 0, 0, 0);
	}

	sprintf(args, "setmode %d %d 32", xdim, ydim);
	ret = rcall(fb->rp, 0, args);

	if (!ret) {
		return 1;
	}

	if (strcmp(ret, "T")) {
		// setting failed
		return 1;
	}

	return fb_resize(fb, xdim, ydim);
}
