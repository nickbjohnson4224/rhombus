/*
 * Copyright (C) 2011 Jaagup Repan <jrepan at gmail.com>
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

#include "wmanager.h"
#include <rho/graph.h>
#include <rho/natio.h>
#include <rho/page.h>
#include <stdlib.h>
#include <string.h>

uint32_t *screen;
int screen_width, screen_height;

void resize_screen(int width, int height) {
	free(screen);

	screen_width = width;
	screen_height = height;

	screen = malloc(screen_width * screen_height * 4);
	memset(screen, 0, screen_width * screen_height * 4);
	rp_share(vgafd, screen, screen_width * screen_height * 4, 0, PROT_READ);
	rp_sync(vgafd);

	if (panel) {
		resize_window(panel, screen_width, panel->height, true);
	}

	update_tiling();
}

void update_screen(int x1, int y1, int x2, int y2) {
	struct window_t *window;

	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 > screen_width) x2 = screen_width;
	if (y2 > screen_height) y2 = screen_height;
	if (x1 > x2) return;
	if (y1 > y2) return;

	for (int x = x1; x < x2; x++) {
		for (int y = y1; y < y2; y++) {
			screen[x + y * screen_width] = COLOR_BLACK;
		}
	}

	for (window = windows; window; window = window->next) {
		draw_window(window, x1, y1, x2, y2);
	}
	draw_cursor(x1, y1, x2, y2);

	rcall(vgafd, "syncrect %i %i %i %i", x1, y1, x2 - x1, y2 - y1);
}

void blit_bitmap(const uint32_t *bitmap, int tox, int toy, int width, int height, int x1, int y1, int x2, int y2) {
	double alpha;
	uint8_t red, green, blue;
	int screen_index, bitmap_index;
	int x, y;

	for (y = toy >= y1 ? toy : y1; y < toy + height && y < y2; y++) {
		for (x = tox >= x1 ? tox : x1; x < tox + width && x < x2; x++) {
			screen_index = x + y * screen_width;
			bitmap_index = (x - tox) + (y - toy) * width;
			alpha = PIX_A(bitmap[bitmap_index]) / 255.0;
			red   = alpha * PIX_R(bitmap[bitmap_index]) + (1 - alpha) * PIX_R(screen[screen_index]);
			green = alpha * PIX_G(bitmap[bitmap_index]) + (1 - alpha) * PIX_G(screen[screen_index]);
			blue  = alpha * PIX_B(bitmap[bitmap_index]) + (1 - alpha) * PIX_B(screen[screen_index]);
			screen[screen_index] = COLOR(red, green, blue);
		}
	}
}
