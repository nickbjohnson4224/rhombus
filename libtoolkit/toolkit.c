/*
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <rdi/robject.h>
#include "toolkit.h"
#include <rho/natio.h>
#include <stdlib.h>
#include <string.h>
#include "private.h"

struct attributes_list {
	char *name;
	uint32_t value;
	struct attributes_list *next;
};

static uint64_t wmanager;
static struct attributes_list *theme_attributes;
char *__rtk_theme_path;

static void toolkit_graph_event(rp_t source, int argc, char **argv) {
	
	if (source != wmanager) return;
	if (argc != 4) return;

	if (!strcmp(argv[1], "resize")) {
		__rtk_set_window_size(__rtk_window, atoi(argv[2]), atoi(argv[3]));
	}
}

static void toolkit_key_event(rp_t source, int argc, char **argv) {
	if (source != wmanager) return;

	if (!strcmp(argv[1], "press") && argc == 3) {
		widget_event(__rtk_window->widget, "key_press", argc - 2, argv + 2);
		update_window(__rtk_window);
	}
	if (!strcmp(argv[1], "release") && argc == 3) {
		widget_event(__rtk_window->widget, "key_release", argc - 2, argv + 2);
		update_window(__rtk_window);
	}
}

static void toolkit_mouse_event(rp_t source, int argc, char **argv) {
	if (source != wmanager) return;

	if (!strcmp(argv[1], "delta") && argc == 4) {
		widget_event(__rtk_window->widget, "mouse_move", argc - 2, argv + 2);
		update_window(__rtk_window);
	}
	if (!strcmp(argv[1], "button") && argc == 3) {
		widget_event(__rtk_window->widget, "mouse_button", argc - 2, argv + 2);
		update_window(__rtk_window);
	}
}

int init_toolkit() {
	wmanager = fs_find("/sys/wmanager");
	if (!wmanager) {
		return 1;
	}

	if (__rtk_init_freetype()) {
		return 1;
	}

	if (set_theme(NULL)) {
		return 1;
	}

	event_hook("graph", toolkit_graph_event);
	event_hook("key",   toolkit_key_event);
	event_hook("mouse", toolkit_mouse_event);

	return 0;
}

int set_theme(const char *theme) {
	char name[256];
	uint32_t value;
	char *path, filename[256], fontpath[256];
	FILE *file;
	struct attributes_list *ptr, *item, *list = NULL;

	if (!theme) {
		return set_theme("default");
	}

	path = saprintf("/etc/themes/%s", theme);
	sprintf(filename, "%s/attributes.txt", path);
	sprintf(fontpath, "%s/font.ttf", path);

	file = fopen(filename, "r");
	if (!file) {
		free(path);
		return 1;
	}

	if (__rtk_set_default_font(fontpath)) {
		free(path);
		fclose(file);
		return 1;
	}

	while (!feof(file)) {
		if (fscanf(file, "%s = %i\n", name, &value) != 2) {
			continue;
		}

		item = malloc(sizeof(struct attributes_list));
		item->next = list;
		list = item;
		item->name = strdup(name);
		item->value = value;
	}

	if (__rtk_theme_path) {
		free(__rtk_theme_path);
		item = theme_attributes;
		while (item) {
			ptr = item;
			item = item->next;
			free(ptr);
		}
	}
	fclose (file);

	__rtk_theme_path = path;
	theme_attributes = list;
	return 0;
}

uint32_t __rtk_get_theme_attribute(const char *name) {
	struct attributes_list *list;

	for (list = theme_attributes; list; list = list->next) {
		if (!strcmp(list->name, name)) {
			return list->value;
		}
	}

	return 0;
}
