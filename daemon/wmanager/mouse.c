/*
 * Copyright (C) 2011 Jaagup Repan
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
#include <natio.h>

const size_t cursor_width = 3, cursor_height = 3;
const uint8_t cursor_bitmap[3 * 3 * 4] = {
	0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00
};

int mousex, mousey;
int mouseclickx, mouseclicky;
int mousebuttons;

void mouse_move(int16_t dx, int16_t dy) {
	struct window_t *window;

	mousex += dx;
	mousey += dy;

	if (mousex < 0) mousex = 0;
	if (mousey < 0) mousey = 0;
	if (mousex >= (int) screen_width) mousex = screen_width - 1;
	if (mousey >= (int) screen_height) mousey = screen_height - 1;

	if (!mousebuttons) { // activate window
		active_window = NULL;
		for (window = windows; window; window = window->next) {
			if (window->x <= mousex && mousex <= window->x + (int) window->width &&
				window->y <= mousey && mousey <= window->y + (int) window->height) {
				active_window = window;
			}
		}
	}

	if (active_window && (mousebuttons & 1)) { // move window
		active_window->x += mousex - mouseclickx;
		active_window->y += mousey - mouseclicky;
		mouseclickx = mousex;
		mouseclicky = mousey;
	}

	draw_cursor();
//	sync(vgafd); //fixme: causes freeze
}

void mouse_click(int buttons) {
	mousebuttons |= buttons;
	mouseclickx = mousex;
	mouseclicky = mousey;
}

void mouse_release(int buttons) {
	mousebuttons &= ~buttons;
}

void draw_cursor() {
	blit_bitmap(cursor_bitmap, mousex, mousey, cursor_width, cursor_height);
}
