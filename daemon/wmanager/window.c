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
#include <mutex.h>

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

int add_window(uint32_t id) {
	struct window_t *list;
	struct window_t *window;

	if (find_window(id, 0)) {
		return -1;
	}

	window = malloc(sizeof(struct window_t));
	window->id = id;
	window->owner = 0;
	window->flags = 0;
	window->x = window->y = 0;
	window->width = window->height = 0;
	window->bitmap = NULL;
	window->mutex = false;
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
	for (window = windows; window; window = window->next, prev = window) {
		if (window->id == id && window->owner == owner) {
			if (window == active_window) {
				active_window = NULL;
			}
			if (prev) {
				prev->next = window->next;
			}
			else {
				windows = window->next;
			}
			free(window);
			return 0;
		}
	}
	return -1;
}

void draw_window(struct window_t *window, int x1, int y1, int x2, int y2) {
	mutex_spin(&window->mutex);

	/* content */
	if (window->bitmap) {
		blit_bitmap(window->bitmap, window->x, window->y, window->width, window->height, x1, y1, x2, y2);
	}
		
	/* decorations */
	for (size_t x = window->x > x1 ? window->x - 1 : x1;
			(x < (window->x + window->width + 1)) && (x < (size_t) x2); x++) {
		if (window->y > y1) {
			for (int c = 0; c < 3; c++) {
				screen[(x + (window->y - 1) * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
		if (window->y + window->height < (size_t) y2) {
			for (int c = 0; c < 3; c++) {
				screen[(x + (window->y + window->height) * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
	}
	for (size_t y = window->y >= y1 ? window->y : y1;
			(y < (window->y + window->height)) && (y < (size_t) y2); y++) {
		if (window->x > x1) {
			for (int c = 0; c < 3; c++) {
				screen[(window->x - 1 + y * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
		if (window->x + window->width < (size_t) x2) {
			for (int c = 0; c < 3; c++) {
				screen[(window->x + window->width + y * screen_width) * 4 + c] =
					(window == active_window && c != 2) ? 0 : 0xff;
			}
		}
	}

	mutex_free(&window->mutex);
}
