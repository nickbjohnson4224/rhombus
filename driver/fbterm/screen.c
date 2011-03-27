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

#include <stdlib.h>
#include <stdio.h>
#include <graph.h>

struct screen screen = {
	NULL,
	NULL,
	COLOR_WHITE,
	COLOR_BLACK,
	0,
	0,
};

int screen_resize(uint32_t x, uint32_t y) {
	
	if (screen.cell) {
		free(screen.cell);
	}

	screen.cell = malloc(sizeof(struct cell) * x * y);
	screen.w = x;
	screen.h = y;

	screen_clear();

	return 0;
}

int screen_print(int x, int y, uint32_t c) {

	if (x < 0 || y < 0 || x >= screen.w || y >= screen.h) {
		return 1;
	}

	screen.cell[x + y * screen.w].ch = c;
	screen.cell[x + y * screen.w].fg = screen.fg;
	screen.cell[x + y * screen.w].fg = screen.bg;

	return 0;
}

int screen_clear(void) {
	int x, y;

	screen.fg = COLOR_WHITE;
	screen.bg = COLOR_BLACK;

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
		draw_cell(screen.font, &screen.cell[i], 
			(i % screen.w) * screen.font->w, (i / screen.w) * screen.font->h);
	}

	fb_flip(fb);

	return 0;
}
