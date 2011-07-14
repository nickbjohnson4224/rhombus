/*
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
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

#include "window.h"
#include <graph.h>
#include <stdlib.h>
#include "widget.h"

struct window *create_window(const char *widget) {
	struct window *window = malloc(sizeof(struct window));
	int width, height;

	if (!window) {
		return NULL;
	}

	window->fb = fb_createwindow();
	if (!window->fb) {
		free(window);
		return NULL;
	}
	fb_getmode(window->fb, &width, &height);

	window->widget = add_widget(widget, NULL, window, 0, 0, width, height);
	if (!window->widget) {
		free(window);
		return NULL;
	}
	window->widget->window = window;

	redraw_window(window);

	return window;
}

void destroy_window(struct window *window) {
	free_widget(window->widget);
	fb_free(window->fb);
	free(window);
}

static void draw_window(struct window *window, bool force) {
	draw_widget(window->widget, force);
	fb_flip(window->fb);
}

void redraw_window(struct window *window) {
	draw_window(window, true);
}

void update_window(struct window *window) {
	draw_window(window, false);
}
