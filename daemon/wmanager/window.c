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

struct window_t *windows;
struct window_t *active_window;

// if owner == 0, then any owner matches
struct window_t *find_window(uint32_t id, uint32_t owner) {
	struct window_t *window;
	for (window = windows; window; window = window->next) {
		if (window->id == id && (owner == 0 || window->owner == owner)) {
			return window;
		}
	}
	return NULL;
}

int add_window(uint32_t id, uint32_t owner) {
	struct window_t *list;
	struct window_t *window;

	if (find_window(id, 0)) {
		return -1;
	}

	window = malloc(sizeof(struct window_t));
	window->id = id;
	window->owner = owner;
	window->x = window->y = 0;
	window->width = window->height = 0;
	window->bitmap = NULL;
	window->next = NULL;

	if (windows) {
		for (list = windows; list->next; list = list->next);
		list->next = window;
	}
	else {
		windows = window;
	}

	return 0;
}

int remove_window(uint32_t id, uint32_t owner) {
	struct window_t *window, *prev = NULL;
	for (window = windows; window; window = window->next) {
		if (window->id == id && window->owner == owner) {
			if (prev) {
				prev->next = window->next;
			}
			else {
				windows = window->next;
			}
			free(window);
			return 0;
		}
		prev = window;
	}
	return -1;
}

void draw_window(struct window_t *window) {
	/* content */
	if (window->bitmap) {
		blit_bitmap(window->bitmap, window->x, window->y, window->width, window->height);
	}
		
	/* decorations */
	for (size_t x = window->x >= 1 ? window->x - 1 : 0;
			(x < (window->x + window->width + 1)) && (x < screen_width); x++) {
		if (window->y >= 1) {
			for (int c = 0; c < 3; c++) {
				screen[(x + (window->y - 1) * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
		if (window->y + window->height < screen_height) {
			for (int c = 0; c < 3; c++) {
				screen[(x + (window->y + window->height) * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
	}
	for (size_t y = window->y >= 0 ? window->y : 0;
			(y < (window->y + window->height)) && (y < screen_height); y++) {
		if (window->x >= 1) {
			for (int c = 0; c < 3; c++) {
				screen[(window->x - 1 + y * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
		if (window->x + window->width < screen_width) {
			for (int c = 0; c < 3; c++) {
				screen[(window->x + window->width + y * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
	}
}

int set_window_size(uint32_t id, uint32_t owner, size_t width, size_t height) {
	struct window_t *window = find_window(id, owner);
	if (!window) {
		return -1;
	}

	window->bitmap = NULL;
	window->width = width;
	window->height = height;
	return 0;
}

int set_window_bitmap(uint32_t id, uint32_t owner, uint8_t *address, size_t size) {
	struct window_t *window = find_window(id, owner);
	if (!window) {
		return -1;
	}
	if (window->bitmap) {
		return -1;
	}
	if (size != window->height * window->width * 4) {
		return -1;
	}
	window->bitmap = address;
	return 0;
}

void blit_bitmap(const uint8_t *bitmap, int tox, int toy, size_t width, size_t height) {
	for (size_t x = tox >= 0 ? tox : 0; x < tox + width && x < screen_width; x++) {
		for (size_t y = toy >= 0 ? toy : 0; y < toy + height && y < screen_height; y++) {
			size_t screen_index = (x + y * screen_width) * 4;
			size_t bitmap_index = ((x - tox) + (y - toy) * width) * 4;
			double alpha = bitmap[bitmap_index + 3] / 255.0;
			for (int c = 0; c < 3; c++) {
				screen[screen_index + c] = (1 - alpha) * screen[screen_index + c] +
					alpha * bitmap[bitmap_index + c];
			}
		}
	}
}
