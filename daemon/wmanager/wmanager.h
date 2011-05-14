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

#ifndef WMANAGER_H
#define WMANAGER_H

#include <stdint.h>
#include <stdbool.h>

enum window_flags {
	LISTEN_EVENTS = 0x1,
	CONSTANT_SIZE = 0x2,
	FLOATING	  = 0x4
};

struct window_t {
	uint32_t id;
	uint32_t owner;
	int flags;
	int x, y;
	size_t width, height;
	uint8_t *bitmap;
	bool mutex;
	struct window_t *next, *prev;
};

extern uint64_t vgafd;
extern uint8_t *screen;
extern size_t screen_width, screen_height;
extern struct window_t *windows;
extern struct window_t *active_window;
extern struct window_t *main_window;
extern bool winkey;

struct window_t *find_window(uint32_t id, uint32_t owner);
int add_window(uint32_t id);
int remove_window(uint32_t id, uint32_t owner);
void draw_window(struct window_t *window, int x1, int y1, int x2, int y2);
void resize_window(struct window_t *window, int width, int height, bool notify);
void bring_to_front(struct window_t *window);
void update_decorations(struct window_t *window);

void resize_screen(size_t width, size_t height);
void update_screen(int x1, int y1, int x2, int y2);
void blit_bitmap(const uint8_t *bitmap, int tox, int toy, size_t width, size_t height, int x1, int y1, int x2, int y2);

void mouse_move(int16_t dx, int16_t dy);
void mouse_buttons(int buttons);
void draw_cursor(int x1, int y1, int x2, int y2);
void activate_window();

void update_tiling();

#endif
