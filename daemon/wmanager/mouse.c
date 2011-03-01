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
#include <page.h>
#include <stdlib.h>

const size_t cursor_width = 3, cursor_height = 3;
const uint8_t cursor_bitmap[3 * 3 * 4] = {
	0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xff,0xff,0xff,0xff, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00
};

int mousex, mousey;
int mousebuttons;

void mouse_move(int16_t dx, int16_t dy) {
	struct window_t *window;

	mousex += dx;
	mousey += dy;

	if (mousex < 0) mousex = 0;
	if (mousey < 0) mousey = 0;
	if (mousex >= (int) screen_width) mousex = screen_width - 1;
	if (mousey >= (int) screen_height) mousey = screen_height - 1;

	if (!mousebuttons) {
		// activate window
		active_window = NULL;
		for (window = windows; window; window = window->next) {
			if (window->x - 1 <= mousex && mousex <= window->x + (int) window->width &&
				window->y - 1 <= mousey && mousey <= window->y + (int) window->height) {
				active_window = window;
			}
		}
	}

	if (active_window) {
		if (mousebuttons & 1) {
			// move window
			active_window->x += dx;
			active_window->y += dy;
		}
		else if ((mousebuttons & 2) && !(active_window->flags & CONSTANT_SIZE)) {
			// resize window
			page_free(active_window->bitmap, active_window->width * active_window->height * 4);
			active_window->bitmap = NULL;
			if (mousex > active_window->x + 10) {
				active_window->width += dx;
			}
			if (mousey > active_window->y + 10) {
				active_window->height += dy;
			}
			mousex = active_window->x + active_window->width;
			mousey = active_window->y + active_window->height;
			if (active_window->flags & LISTEN_EVENTS) {
				event(RP_CONS(active_window->owner, 0), active_window->width << 16 | active_window->height);
			}
		}
	}
	
	update_screen(mousex - (dx > 0 ? dx : 0), mousey - (dy > 0 ? dy : 0), mousex + cursor_width + abs(dx), mousey + cursor_height + abs(dy));
}

void mouse_click(int buttons) {
	mousebuttons |= buttons;
}

void mouse_release(int buttons) {
	mousebuttons &= ~buttons;
}

void draw_cursor(int x1, int y1, int x2, int y2) {
	blit_bitmap(cursor_bitmap, mousex, mousey, cursor_width, cursor_height, x1, y1, x2, y2);
}
