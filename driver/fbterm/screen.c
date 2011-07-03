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

#include "fbterm.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <graph.h>

struct screen screen = {
	8,
	0,
	0,
	NULL,
	COLOR_WHITE,
	COLOR_BLACK,
	0,
	0,
};

int screen_resize(uint32_t x, uint32_t y) {
	struct cell *old;
	int old_w, old_h;
	int i, j;
	
	if (screen.cell) {
		old = screen.cell;
	}

	old_w = screen.w;
	old_h = screen.h;

	screen.w = x / screen.cell_width;
	screen.h = y / screen.cell_height;
	screen.cell = malloc(sizeof(struct cell) * screen.w * screen.h);

	fb_resize(fb, x, y);
	screen_clear();

	if (old) {
		for (i = 0; i < screen.w && i < old_w; i++) {
			for (j = 0; j < screen.h && j < old_h; j++) {
				screen_print(i, j, old[i + j * old_w].ch);
			}
		}

		free(old);
	}

	return 0;
}

int screen_print(int x, int y, uint32_t c) {
	struct cell *cell;

	if (x < 0 || y < 0 || x >= screen.w || y >= screen.h) {
		return 1;
	}

	cell = &screen.cell[x + y * screen.w];
	if (cell->ch != c || cell->fg != screen.fg || cell->bg != screen.bg) {
		cell->dirty = 1;
	}
	
	cell->ch = c;
	cell->fg = screen.fg;
	cell->bg = screen.bg;

	return 0;
}

int screen_scroll(void) {
	uint32_t fg, bg;
	int x, y;

	fg = screen.fg;
	bg = screen.bg;
	
	for (x = 0; x < screen.w; x++) {
		for (y = 0; y < screen.h - 1; y++) {
			screen.fg = screen.cell[x + y * screen.w].fg;
			screen.bg = screen.cell[x + y * screen.w].bg;
			screen_print(x, y, screen.cell[x + (y + 1) * screen.w].ch);
		}
		screen.fg = COLOR_WHITE;
		screen.bg = COLOR_BLACK;
		screen_print(x, y, ' ');
	}

	screen.fg = fg;
	screen.bg = bg;

	return 0;
}

int screen_clear(void) {
	int x, y;

	for (x = 0; x < screen.w; x++) {
		for (y = 0; y < screen.h; y++) {
			screen_print(x, y, ' ');
		}
	}

	return 0;
}

int screen_flip(void) {
	int i;

	for (i = 0; i < screen.w * screen.h; i++) {
		if (screen.cell[i].dirty) {
			draw_cell(&screen.cell[i], 
				(i % screen.w) * screen.cell_width, (i / screen.w) * screen.cell_height);
			screen.cell[i].dirty = 0;
		}
	}

	fb_flip(fb);

	return 0;
}

int screen_sync(void) {
	int i;

	for (i = 0; i < screen.w * screen.h; i++) {
		draw_cell(&screen.cell[i], 
			(i % screen.w) * screen.cell_width, (i / screen.w) * screen.cell_height);
		screen.cell[i].dirty = 0;
	}

	fb_flip(fb);

	return 0;
}
