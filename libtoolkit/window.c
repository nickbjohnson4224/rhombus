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
#include <ctype.h>
#include <rho/graph.h>
#include <rho/natio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "widget.h"
#include "private.h"

struct window *__rtk_window; //todo: support for multiple windows
static const int stack_size = 16;

struct window *create_window_from_widget(const char *widget) {
	struct window *window = malloc(sizeof(struct window));
	int width, height;

	if (!window) {
		return NULL;
	}

	window->handler = NULL;
	window->fb = fb_createwindow();
	if (!window->fb) {
		free(window);
		return NULL;
	}
	fb_getmode(window->fb, &width, &height);

	window->widget = __rtk_add_widget(widget, NULL, window, 0, 0, width, height);
	if (!window->widget) {
		free(window);
		return NULL;
	}
	window->widget->window = window;

	event_subscribe(get_resource_pointer(window));
	draw_window(window);

	__rtk_window = window;
	return window;
}

static char *parse_arguments(char *string, char *name, char *value) {
	char *start, *end, *middle;

	start = string;
	while (isspace(*start)) {
		start++;
	}

	if (start >= string + strlen(string)) {
		return NULL;
	}

	middle = strchr(start, '=') + 1;
	if (!middle) {
		return NULL;
	}

	end = strchr(middle, ' ');
	if (!end) {
		end = string + strlen(string) - 1;
	}

	strncpy(name, start, middle - start - 1);
	strncpy(value, middle, end - middle);
	name[middle - start - 1] = 0;
	value[end - middle] = 0;

	return end;
}

#define WINDOW_FLAG(x,y) \
				if (!strcmp(name, x)) { \
					if (!strcmp(name, "true")) { \
						flags |= y; \
					} \
					if (!strcmp(name, "false")) { \
						flags &= ~y; \
					} \
				} \

struct window *create_window_from_file(const char *filename) {
	FILE *file = fopen(filename, "r");
	struct window *window;
	struct widget *stack[stack_size + 1];
	struct widget **stack_ptr = stack - 1;
	char buffer[256], name[256], value[256];
	char *start, *end;
	bool first = true;
	int width = 0, height = 0, flags = 0;

	if (!file) {
		return NULL;
	}

	while (stack_ptr >= stack || first) {
		fgets(buffer, sizeof(buffer), file);

		start = buffer;
		while (isspace(*start))
			start++;
		end = strchr(buffer, ' ');
		if (!end) {
			end = buffer + strlen(buffer) - 1;
		}
		strncpy(name, start, end - start);
		name[end - start] = 0;

		if (!strcmp(name, "windowoptions")) {
			while ((end = parse_arguments(end, name, value))) {
				if (!strcmp(name, "width")) {
					width = atoi(value);
				}
				if (!strcmp(name, "height")) {
					height = atoi(value);
				}
				WINDOW_FLAG("constant_size", CONSTANT_SIZE);
				WINDOW_FLAG("floating", FLOATING);
			}
		}
		else if (!strcmp(name, "end")) {
			stack_ptr--;
		}
		else {
			if (first) {
				window = create_window_from_widget(name);
				if (!window) {
					fclose(file);
					return NULL;
				}

				if (width || height) {
					resize_window(window, width, height);
				}
				if (flags) {
					add_window_flags(window, flags);
				}

				*(++stack_ptr) = window->widget;
				first = false;
			}
			else {
				widget_call(*stack_ptr, "add_widget", name, NULL);
				*(stack_ptr + 1) = (*stack_ptr)->children;
				stack_ptr++;
			}
			
			if (stack_ptr - stack >= stack_size) {
				fclose(file);
				destroy_window(window);
				return NULL;
			}
			
			while ((end = parse_arguments(end, name, value))) {
				if (!strcmp(name, "name")) {
					set_name(*stack_ptr, value);
				}
				else {
				   	if (!strcmp(name, "width")) {
						(*stack_ptr)->width = atoi(value);
					}
					if (!strcmp(name, "height")) {
						(*stack_ptr)->height = atoi(value);
					}

					set_attribute_string(*stack_ptr, name, value);
				}
			}
		}
	}

	fclose(file);
	draw_window(window);
	return window;
}

struct window *create_window_from_store(const char *window) {
	char *filename = saprintf("/etc/windows/%s.txt", window);
	struct window *ret = create_window_from_file(filename);
	free(filename);
	return ret;
}

void destroy_window(struct window *window) {
	__rtk_free_widget(window->widget);
	fb_free(window->fb);
	free(window);
}

static void __draw_window(struct window *window, bool force) {
	__rtk_draw_widget(window->widget, force);
	fb_flip(window->fb);
}

void draw_window(struct window *window) {
	__draw_window(window, true);
}

void update_window(struct window *window) {
	__draw_window(window, false);
}

void __rtk_set_window_size(struct window *window, int width, int height) {
	fb_resize(window->fb, width, height);
	set_size(window->widget, width, height);
	draw_window(window);
}


int resize_window(struct window *window, int width, int height) {
	if (fb_setmode(window->fb, width, height)) {
		return 1;
	}

	__rtk_set_window_size(window, width, height);
	return 0;
}

void get_window_size(struct window *window, int *width, int *height) {
	fb_getmode(window->fb, width, height);
}

void window_register(struct window *window, handler_t handler) {
	window->handler = handler;
}

static int get_window_flags(struct window *window) {
	char *ret = rcall(get_resource_pointer(window), "getwindowflags");
	int flags;

	sscanf(ret, "%i", &flags);
	return flags;
}

static void set_window_flags(struct window *window, enum window_flags flags) {
	rcall(get_resource_pointer(window), "setwindowflags %i", flags);
}

void add_window_flags(struct window *window, enum window_flags flags) {
	int curflags = get_window_flags(window);
	curflags |= flags;
	set_window_flags(window, curflags);
}

void clear_window_flags(struct window *window, enum window_flags flags) {
	int curflags = get_window_flags(window);
	curflags &= ~flags;
	set_window_flags(window, curflags);
}

struct widget *find_widget(struct window *window, const char *name) {
	return find_child(window->widget, name);
}

uint64_t get_resource_pointer(struct window *window) {
	return window->fb->rp;
}
