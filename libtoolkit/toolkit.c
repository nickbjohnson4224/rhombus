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

#include "toolkit.h"
#include <natio.h>
#include <stdlib.h>
#include <string.h>
#include "private.h"

static uint64_t wmanager;

void toolkit_graph_event(uint64_t source, int argc, char **argv) {
	
	if (source != wmanager) return;
	if (argc != 4) return;

	if (!strcmp(argv[1], "resize")) {
		__rtk_set_window_size(__rtk_window, atoi(argv[2]), atoi(argv[3]));
	}
}

void toolkit_key_event(uint64_t source, int argc, char **argv) {
	if (source != wmanager) return;

	if (!strcmp(argv[1], "press") && argc == 3) {
		widget_event(__rtk_window->widget, "key_press", argc - 2, argv + 2);
	}
	if (!strcmp(argv[1], "release") && argc == 3) {
		widget_event(__rtk_window->widget, "key_release", argc - 2, argv + 2);
	}
}

void toolkit_mouse_event(uint64_t source, int argc, char **argv) {
	if (source != wmanager) return;

	if (!strcmp(argv[1], "delta") && argc == 4) {
		widget_event(__rtk_window->widget, "mouse_move", argc - 2, argv + 2);
	}
	if (!strcmp(argv[1], "button") && argc == 3) {
		widget_event(__rtk_window->widget, "mouse_button", argc - 2, argv + 2);
	}
}

int init_toolkit() {
	wmanager = fs_find("/sys/wmanager");
	if (!wmanager) {
		return 1;
	}

	event_set("graph", toolkit_graph_event);
	event_set("key",   toolkit_key_event);
	event_set("mouse", toolkit_mouse_event);

	return __rtk_init_freetype();
}
