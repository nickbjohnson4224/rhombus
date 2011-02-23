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
#include <stdlib.h>
#include "list.h"

struct window_t *windows;
struct window_t *active_window;

int add_window(uint32_t id) {
	struct window_t *window;
	LIST_FIND(window, window, item->id == id)
	if (window) {
		return -1;
	}

	window = malloc(sizeof(struct window_t));
	window->id = id;
	window->x = window->y = 0;
	window->width = window->height = window->bitmap = 0;
	window->next = window->prev = NULL;

	LIST_ADD(window)
	return 0;
}

int remove_window(uint32_t id) {
	struct window_t *window;
	LIST_FIND(window, window, item->id == id)
	if (!window) {
		return -1;
	}
	LIST_REMOVE(window)
	return 0;
}

void draw_window(struct window_t *window) {
	/* content */
	struct bitmap_t *bitmap;
	LIST_FIND(bitmap, bitmap, item->id == window->bitmap)
	if (bitmap && bitmap->size >= window->width * window->height * 4) {
		blit_bitmap(bitmap->address, window->x, window->y, window->width, window->height);
	}
		
	/* decorations */
	for (size_t x = window->x >= 1 ? window->x - 1 : 0;
			(x < (window->x + window->width + 1)) && (x < screen_width); x++) {
		if (window->y >= 1) {
			for (int c = 0; c < 3; c++) {
				screen[(x + (window->y - 1) * screen_width) * 3 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
		if (window->y + window->height < screen_height) {
			for (int c = 0; c < 3; c++) {
				screen[(x + (window->y + window->height) * screen_width) * 3 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
	}
	for (size_t y = window->y >= 0 ? window->y : 0;
			(y < (window->y + window->height)) && (y < screen_height); y++) {
		if (window->x >= 1) {
			for (int c = 0; c < 3; c++) {
				screen[(window->x - 1 + y * screen_width) * 3 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
		if (window->x + window->width < screen_width) {
			for (int c = 0; c < 3; c++) {
				screen[(window->x + window->width + y * screen_width) * 3 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
	}
}

int set_window_size(uint32_t id, size_t width, size_t height) {
	struct window_t *window;
	struct bitmap_t *bitmap;

	LIST_FIND(window, window, item->id == id)
	if (!window) {
		return -1;
	}

	LIST_FIND(bitmap, bitmap, item->id == window->bitmap)
	if (bitmap && bitmap->size != width * height * 4) {
		return -1;
	}

	window->width = width;
	window->height = height;
	return 0;
}

int set_window_bitmap(uint32_t id, uint32_t bitmap_id) { 
	struct window_t *window;
	struct bitmap_t *bitmap;

	LIST_FIND(window, window, item->id == id)
	if (!window) {
		return -1;
	}

	LIST_FIND(bitmap, bitmap, item->id == bitmap_id)
	if (!bitmap) {
		return -1;
	}
	if (bitmap->size != window->width * window->height * 4) {
		return -1;
	}

	window->bitmap = bitmap_id;
	return 0;
}
