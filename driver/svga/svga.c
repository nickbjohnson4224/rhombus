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

#include "svga.h"

#include <string.h>
#include <stdlib.h>
#include <graph.h>
#include <page.h>

#include <rdi/arch.h>

struct svga svga;
struct svga_mode *modelist;
int modelist_count;

int svga_init(void) {
	uint16_t *modelist_raw;
	int i, j;

	vm86_setup();

	if (vbe_readctrl()) {
		return 1;
	}

	modelist_raw = (void*) ((svga_ctrl->mode_ptr & 0xFFFF) | (svga_ctrl->mode_ptr >> 12));
	for (i = 0; modelist_raw[i] != 0xFFFF; i++);
	modelist = malloc(sizeof(struct svga_mode) * i);
	
	for (i = j = 0; modelist_raw[i] != 0xFFFF; i++) {
		vbe_readmode(modelist_raw[i]);
		
		if (svga_mode->mem_model != 6) continue;
		
		modelist[j].vesa_mode = modelist_raw[i];
		modelist[j].w = svga_mode->xres;
		modelist[j].h = svga_mode->yres;
		modelist[j].d = svga_mode->depth;
		j++;
	}

	modelist_count = j;

	return 0;
}

int svga_find_mode(int width, int height, int depth) {
	int best, i;
	int diff, mindiff;
	int x, y;

	mindiff = width * height;
	for (i = 0; i < modelist_count; i++) {
		diff = (width * height) - (modelist[i].w * modelist[i].h);
		if (diff < 0) diff = -diff;
		if (diff < mindiff) {
			best = i;
			mindiff = diff;
		}
	}

	x = modelist[best].w;
	y = modelist[best].h;

	mindiff = depth;
	for (i = 0; i < modelist_count; i++) {
		if (modelist[i].w == x && modelist[i].h == y) {
			diff = depth - modelist[i].d;
			if (diff < 0) diff = -diff;
			if (diff < mindiff) {
				best = i;
				mindiff = diff;
			}
		}
	}

	return modelist[best].vesa_mode;
}

int svga_set_mode(int mode) {
	char *event;
	
	if (vbe_readmode(mode)) {
		return 1;
	}

	svga.w  = svga_mode->xres;
	svga.h  = svga_mode->yres;
	svga.d  = svga_mode->depth;
	svga.p  = svga_mode->scan_bytes;
	svga.pw = (svga.d & 0x07) ? (svga.d >> 3) + 1 : svga.d >> 3;
	
	svga.r_size = svga_mode->red_size;
	svga.r_shft = svga_mode->red_off;
	svga.g_size = svga_mode->green_size;
	svga.g_shft = svga_mode->green_off;
	svga.b_size = svga_mode->blue_size;
	svga.b_shft = svga_mode->blue_off;

	if (svga_mode->attributes & 0x8) {
		// use linear mode
		
		svga.mode = 1;
		svga.linear = valloc(svga_ctrl->memory << 16);
		page_phys(svga.linear, svga_ctrl->memory << 16, PROT_READ | PROT_WRITE, svga_mode->linear_ptr);
	}
	else {
		// use windowed mode

		svga.mode = 0;
		svga.window0 = (void*) (svga_mode->seg_a << 4);
		svga.window1 = (void*) (svga_mode->seg_b << 4);
		svga.window_size = svga_mode->winsize << 10;
	}

	if (vbe_setmode(mode, (svga.mode) ? 0x20 : 0x00)) {
		return 1;
	}

	event = saprintf("graph resize %d %d", svga.w, svga.h);
	eventl(event_list, event);
	free(event);
	
	return 0;
}

static int svga_putbyte(uint32_t index, uint8_t byte) {
	static int bank;

	if (svga.mode == 1) {
		svga.linear[index] = byte;
	}
	else if (svga.mode == 0) {
		if ((index < bank * svga.window_size) || (index >= (bank + 1) * svga.window_size)) {
			bank = index / svga.window_size;
			vbe_setbank(0, bank * (svga_mode->winsize / svga_mode->granularity));
		}
	
		index -= bank * svga.window_size;
		svga.window0[index] = byte;
	}

	return 0;
}

static int svga_putpixel(int x, int y, uint32_t pixel) {
	uint32_t cooked;
	uint32_t index;
	uint8_t r, g, b;
	int i;

	index = (x * svga.pw) + (y * svga.p);

	r = PIX_R(pixel);
	g = PIX_G(pixel);
	b = PIX_B(pixel);

	r >>= (8 - svga.r_size);
	g >>= (8 - svga.g_size);
	b >>= (8 - svga.b_size);

	cooked = 0;
	cooked |= r << svga.r_shft;
	cooked |= g << svga.g_shft;
	cooked |= b << svga.b_shft;

	for (i = 0; i < svga.pw; i++) {
		svga_putbyte(index + i, cooked & 0xFF);
		cooked >>= 8;
	}

	return 0;
}

int svga_flip(uint32_t *buffer) {
	int x, y;

	for (y = 0; y < svga.h; y++) {
		for (x = 0; x < svga.w; x++) {
			svga_putpixel(x, y, buffer[x + y * svga.w]);
		}
	}

	return 0;
}

int svga_fliprect(uint32_t *buffer, int x, int y, int w, int h) {
	int x1, y1;

	if ((x < 0) || (y < 0) || (x + w > svga.w) || (y + h > svga.h)) {
		return 1;
	}

	for (y1 = y; y1 < y + h; y1++) {
		for (x1 = x; x1 < x + w; x1++) {
			svga_putpixel(x1, y1, buffer[x1 + y1 * svga.w]);
		}
	}

	return 0;
}
