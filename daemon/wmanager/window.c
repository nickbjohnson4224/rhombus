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
#include <stdlib.h>
#include <rho/mutex.h>
#include <rho/natio.h>
#include <rho/page.h>

struct window_t *windows, *windows_end;
struct window_t *active_window;

void add_to_list(struct window_t *window) {
	window->next = NULL;
	window->prev = windows_end;
	if (windows_end) {
		windows_end->next = window;
	}
	else {
		windows = window;
	}
	windows_end = window;
}

void remove_from_list(struct window_t *window) {
	if (window->next) {
		window->next->prev = window->prev;
	}
	else {
		windows_end = window->prev;
	}
	if (window->prev) {
		window->prev->next = window->next;
	}
	else {
		windows = window->next;
	}
}

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
	window->tags = current_tags;
	window->bitmap = NULL;
	window->mutex = false;

	add_to_list(window);
	update_tiling();

	return 0;
}

int remove_window(uint32_t id, uint32_t owner) {
	struct window_t *window = find_window(id, owner);
	int flags;

	if (!window) {
		return -1;
	}

	if (window == active_window) {
		active_window = NULL;
	}
	flags = window->flags;

	remove_from_list(window);
	free(window);

	if (!(flags & FLOATING)) {
		update_tiling();
	}

	return 0;
}

void draw_window(struct window_t *window, int x1, int y1, int x2, int y2) {
	if (!(window->tags & current_tags)) {
		return;
	}

	mutex_spin(&window->mutex);

	/* content */
	if (window->bitmap) {
		blit_bitmap(window->bitmap, window->x, window->y, window->width, window->height, x1, y1, x2, y2);
	}
		
	/* decorations */
	for (int x = window->x > x1 ? window->x - 1 : x1;
			(x < (window->x + window->width + 1)) && (x < x2); x++) {
		if (window->y > y1) {
			screen[x + (window->y - 1) * screen_width] =
				(window == active_window) ? ACTIVE_WINDOW_BORDER : INACTIVE_WINDOW_BORDER;
		}
		if (window->y + window->height < y2) {
			screen[x + (window->y + window->height) * screen_width] =
				(window == active_window) ? ACTIVE_WINDOW_BORDER : INACTIVE_WINDOW_BORDER;
		}
	}
	for (int y = window->y >= y1 ? window->y : y1;
			(y < (window->y + window->height)) && (y < y2); y++) {
		if (window->x > x1) {
			screen[window->x - 1 + y * screen_width] =
				(window == active_window) ? ACTIVE_WINDOW_BORDER : INACTIVE_WINDOW_BORDER;
		}
		if (window->x + window->width < x2) {
			screen[window->x + window->width + y * screen_width] =
				(window == active_window) ? ACTIVE_WINDOW_BORDER : INACTIVE_WINDOW_BORDER;
		}
	}

	mutex_free(&window->mutex);
}

void resize_window(struct window_t *window, int width, int height, bool notify) {
	char *event_str;

	if (window->width == width && window->height == height) {
		return;
	}

	mutex_spin(&window->mutex);

	if (window->bitmap) {
		page_free(window->bitmap, window->width * window->height * 4);
		window->bitmap = NULL;
	}

	if (width < 10) {
		width = 10;
	}
	if (height < 10) {
		height = 10;
	}

	window->width = width;
	window->height = height;

	if (notify && (window->flags & LISTEN_EVENTS)) {
		event_str = saprintf("graph resize %d %d", window->width, window->height);
		event(RP_CONS(window->owner, 0), event_str);
		free(event_str);
	}

	mutex_free(&window->mutex);

	update_decorations(window);
}

void bring_to_front(struct window_t *window) {
	if (window != windows_end) {
		remove_from_list(window);
		add_to_list(window);
	}
}

void update_decorations(struct window_t *window) {
	update_screen(window->x - 1, window->y - 1, window->x, window->y + window->height + 1);
	update_screen(window->x + window->width, window->y - 1, window->x + window->width + 1, window->y + window->height + 1);
	update_screen(window->x, window->y - 1, window->x + window->width, window->y);
	update_screen(window->x, window->y + window->height, window->x + window->width, window->y + window->height + 1);
}
