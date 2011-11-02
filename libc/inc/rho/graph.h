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

#ifndef __RLIBC_GRAPH_H
#define __RLIBC_GRAPH_H

#include <stdint.h>
#include <rho/mutex.h>

/* pixel format and colors **************************************************/

// channel isolation
#define PIX_R(p) (((p) >>  0) & 0xFF)
#define PIX_G(p) (((p) >>  8) & 0xFF)
#define PIX_B(p) (((p) >> 16) & 0xFF)
#define PIX_A(p) (((p) >> 24) & 0xFF)

#define COLORA(r, g, b, a) \
	((((a) & 0xFF) << 24) | \
	 (((b) & 0xFF) << 16) | \
	 (((g) & 0xFF) <<  8) | \
	 (((r) & 0xFF) <<  0))

#define COLOR(r, g, b) COLORA(r, g, b, 0xFF)

// pure color macros
#define COLOR_WHITE		COLOR(0xFF, 0xFF, 0xFF)
#define COLOR_BLACK 	COLOR(0x00, 0x00, 0x00)
#define COLOR_DGRAY		COLOR(0x55, 0x55, 0x55)
#define COLOR_LGRAY		COLOR(0xAB, 0xAB, 0xAB)
#define COLOR_RED		COLOR(0xFF, 0x00, 0x00)
#define COLOR_YELLOW	COLOR(0xFF, 0xFF, 0x00)
#define COLOR_GREEN		COLOR(0x00, 0xFF, 0x00)
#define COLOR_CYAN		COLOR(0x00, 0xFF, 0xFF)
#define COLOR_BLUE		COLOR(0x00, 0x00, 0xFF)
#define COLOR_MAGENTA	COLOR(0xFF, 0x00, 0xFF)

// nice color macros (from xkcd color survey results)
#define COLOR_NWHITE	COLOR(0xFF, 0xFF, 0xFF)
#define COLOR_NBLACK 	COLOR(0x00, 0x00, 0x00)
#define COLOR_NDGRAY	COLOR(0x55, 0x55, 0x55)
#define COLOR_NLGRAY	COLOR(0xAB, 0xAB, 0xAB)
#define COLOR_NPINK		COLOR(0xFF, 0x81, 0xC0)
#define COLOR_NRED		COLOR(0xE5, 0x00, 0x00)
#define COLOR_NORANGE	COLOR(0xF9, 0x73, 0x06)
#define COLOR_NYELLOW	COLOR(0xFF, 0xFF, 0x14)
#define COLOR_NLIME		COLOR(0x89, 0xFE, 0x05)
#define COLOR_NGREEN	COLOR(0x15, 0xB0, 0x1A)
#define COLOR_NCYAN		COLOR(0x00, 0xFF, 0xFF)
#define COLOR_NBLUE		COLOR(0xFF, 0x81, 0xC0)
#define COLOR_NPURPLE	COLOR(0x7E, 0x1E, 0x9C)
#define COLOR_NMAGENTA	COLOR(0xC2, 0x00, 0x78)
#define COLOR_NBROWN	COLOR(0x65, 0x37, 0x00)

/* framebuffer structure ****************************************************/

struct fb {
	uint64_t rp;      // resource pointer of device
	int fd;

	uint32_t *bitmap; // framebuffer contents
	bool mutex;
	int xdim;
	int ydim;
	int flags;

	// flip acceleration (internal)
	int minx;
	int miny;
	int maxx;
	int maxy;
};

#define FB_SHARED	0x0001	// uses shared memory interface
#define FB_USRBMP	0x0002	// using bitmap from user (don't free)

/* framebuffer operations ***************************************************/

// constructors/destructors
struct fb *fb_cons(uint64_t rp);
struct fb *fb_createwindow();
void       fb_free(struct fb *fb);

// mode setting
int        fb_setmode(struct fb *fb, int xdim, int ydim);
int        fb_getmode(struct fb *fb, int *xdim, int *ydim);

// basic operations
int        fb_flip (struct fb *fb);
int        fb_plot (struct fb *fb, int x, int y, uint32_t color);
int        fb_fill (struct fb *fb, int x, int y, int w, int h, uint32_t color);
int        fb_blit (struct fb *fb, uint32_t *bitmap, int x, int y, int w, int h);

// raw bitmap access
int        fb_lock  (struct fb *fb);
int        fb_unlock(struct fb *fb);
uint32_t  *fb_getbmp(struct fb *fb);
int        fb_setbmp(struct fb *fb, uint32_t *bitmap);
int		   fb_resize(struct fb *fb, int xdim, int ydim);

#endif/*__RLIBC_GRAPH_H*/
