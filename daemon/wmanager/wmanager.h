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

struct window_t {
	uint32_t id;
	uint32_t owner;
	int x, y;
	size_t width, height;
	uint8_t *bitmap;
	struct window_t *next;
};

extern uint64_t vgafd;
extern uint8_t *screen;
extern size_t screen_width, screen_height;
extern struct window_t *windows;
extern struct window_t *active_window;

int add_window(uint32_t id, uint32_t owner);
int remove_window(uint32_t id, uint32_t owner);
void draw_window(struct window_t *window);
int set_window_size(uint32_t id, uint32_t owner, size_t width, size_t height);
int set_window_bitmap(uint32_t id, uint32_t owner, uint8_t *address, size_t size);
void blit_bitmap(const uint8_t *bitmap, int tox, int toy, size_t width, size_t height);

void mouse_move(int16_t dx, int16_t dy);
void mouse_click(int buttons);
void mouse_release(int buttons);
void draw_cursor();

#endif
