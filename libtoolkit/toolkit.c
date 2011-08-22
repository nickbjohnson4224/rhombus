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

struct colors_list {
	char *name;
	uint32_t value;
	struct colors_list *next;
};

static uint64_t wmanager;
static struct colors_list *theme_colors;
char *__rtk_theme_path;

static void toolkit_graph_event(uint64_t source, int argc, char **argv) {
	
	if (source != wmanager) return;
	if (argc != 4) return;

	if (!strcmp(argv[1], "resize")) {
		__rtk_set_window_size(__rtk_window, atoi(argv[2]), atoi(argv[3]));
	}
}

static void toolkit_key_event(uint64_t source, int argc, char **argv) {
	if (source != wmanager) return;

	if (!strcmp(argv[1], "press") && argc == 3) {
		widget_event(__rtk_window->widget, "key_press", argc - 2, argv + 2);
	}
	if (!strcmp(argv[1], "release") && argc == 3) {
		widget_event(__rtk_window->widget, "key_release", argc - 2, argv + 2);
	}
}

static void toolkit_mouse_event(uint64_t source, int argc, char **argv) {
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

	if (set_theme(NULL)) {
		return 1;
	}

	event_set("graph", toolkit_graph_event);
	event_set("key",   toolkit_key_event);
	event_set("mouse", toolkit_mouse_event);

	return __rtk_init_freetype();
}

int set_theme(const char *theme) {
	char name[256];
	uint32_t value;
	char *path, filename[256];
	FILE *file;
	struct colors_list *ptr, *item, *list = NULL;

	if (!theme) {
		return set_theme("default");
	}

	path = saprintf("/etc/themes/%s", theme);
	sprintf(filename, "%s/colors.txt", path);

	file = fopen(filename, "r");
	if (!file) {
		free(path);
		return 1;
	}

	while (!feof(file)) {
		if (fscanf(file, "%s = %i\n", name, &value) != 2) {
			continue;
		}

		item = malloc(sizeof(struct colors_list));
		item->next = list;
		list = item;
		item->name = strdup(name);
		item->value = value;
	}

	if (__rtk_theme_path) {
		free(__rtk_theme_path);
		item = theme_colors;
		while (item) {
			ptr = item;
			item = item->next;
			free(ptr);
		}
	}

	__rtk_theme_path = path;
	theme_colors = list;
	return 0;
}

uint32_t __rtk_get_color(const char *name) {
	struct colors_list *list;

	for (list = theme_colors; list; list = list->next) {
		if (!strcmp(list->name, name)) {
			return list->value;
		}
	}

	return 0;
}
