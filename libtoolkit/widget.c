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

#include "widget.h"
#include <graph.h>
#include <mutex.h>
#include <string.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include "private.h"

#define DEBUG 1

bool __rtk_mutex;

struct widget *add_widget(const char *name, struct widget *parent, struct window *window, int x, int y, int w, int h) {
	struct widget *widget = malloc(sizeof(struct widget));
	struct widget *prev = __rtk_curwidget;
	char *filename;

	if (!widget) {
		return NULL;
	}

	widget->window = window;
	widget->x = x;
	widget->y = y;
	widget->parent = parent;
	widget->children = NULL;
	widget->prev = widget->next = NULL;

	mutex_spin(&__rtk_mutex);
	__rtk_curwidget = widget;

	filename = saprintf("/etc/widgets/%s.lua", name);
	if (luaL_loadfile(__rtk_L, filename)) {
		free(widget);
		free(filename);
		mutex_free(&__rtk_mutex);
		return NULL;
	}
	free(filename);

	if (parent) {
		if (parent->children) {
			parent->children->prev = widget;
		}
		widget->next = parent->children;
		parent->children = widget;
	}

	lua_pushvalue(__rtk_L, -1);
	widget->ref = luaL_ref(__rtk_L, LUA_REGISTRYINDEX);
	lua_pcall(__rtk_L, 0, 0, 0);

	__rtk_curwidget = prev;
	mutex_free(&__rtk_mutex);

	set_size(widget, w, h);

	return widget;
}

void free_widget(struct widget *widget) {
	struct widget *ptr;
	
	for (ptr = widget->children; ptr; ptr = ptr->next) {
		free_widget(ptr);
	}

	if (widget->parent) {
		widget->parent->children = widget->next;
	}
	if (widget->next) {
		widget->next->prev = widget->prev;
	}
	if (widget->prev) {
		widget->prev->next = widget->next;
	}

	mutex_spin(&__rtk_mutex);
	luaL_unref(__rtk_L, LUA_REGISTRYINDEX, widget->ref);
	mutex_free(&__rtk_mutex);
	free(widget);
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))

int draw_widget(struct widget *widget, bool force) {
	struct widget *prev = __rtk_curwidget;
	struct widget *ptr;

	if (!widget->parent) {
		__rtk_curx = __rtk_cury = 0;
		__rtk_curwidth = widget->width;
		__rtk_curheight = widget->height;
	}

	if (force || widget->dirty) {
		mutex_spin(&__rtk_mutex);
		__rtk_curwidget = widget;
	
		lua_rawgeti(__rtk_L, LUA_REGISTRYINDEX, widget->ref);
		lua_getglobal(__rtk_L, "draw");
		if (lua_pcall(__rtk_L, 0, 0, 0)) {
#if DEBUG
			fprintf(stderr, "Toolkit Lua error: %s\n", lua_tostring(__rtk_L, -1));
#else
			lua_pop(__rtk_L, -1);
#endif
			return 1;
		}

		__rtk_curwidget = prev;
		mutex_free(&__rtk_mutex);
	}

	if (force || widget->child_dirty) {
		__rtk_curx += widget->x;
		__rtk_cury += widget->y;
		__rtk_curwidth = MIN(ptr->width, widget->width - ptr->x);
		__rtk_curheight = MIN(ptr->height, widget->height - ptr->y);
		for (ptr = widget->children; ptr; ptr = ptr->next) {
			if (force || ptr->dirty || ptr->child_dirty) {
				draw_widget(ptr, force);
			}
		}
		__rtk_curx -= widget->x;
		__rtk_cury -= widget->y;
	}

	widget->dirty = false;
	widget->child_dirty = false;

	return 0;
}

#undef MIN

void set_position(struct widget *widget, int x, int y) {
	widget->x = x;
	widget->y = y;
}

void get_position(struct widget *widget, int *x, int *y) {
	*x = widget->x;
	*y = widget->y;
}

int set_size(struct widget *widget, int width, int height) {
	widget->width = width;
	widget->height = height;

	return set_attribute_int(widget, "width", width) ||
	       set_attribute_int(widget, "height", height);
}

void get_size(struct widget *widget, int *width, int *height) {
	*width = widget->width;
	*height = widget->height;
}

int __rtk_set_attribute(struct widget *widget) {
	struct widget *prev = __rtk_curwidget;

	__rtk_curwidget = widget;
	lua_rawgeti(__rtk_L, LUA_REGISTRYINDEX, widget->ref);
	lua_getglobal(__rtk_L, "set_attribute");
	lua_insert(__rtk_L, -4);
	lua_insert(__rtk_L, -4);
	if (lua_pcall(__rtk_L, 2, 0, 0)) {
#if DEBUG
		fprintf(stderr, "Toolkit Lua error: %s\n", lua_tostring(__rtk_L, -1));
#else
		lua_pop(__rtk_L, -1);
#endif
		__rtk_curwidget = prev;
		return 1;
	}
	__rtk_curwidget = prev;
	return 0;
} 

int __rtk_get_attribute(struct widget *widget) {
	struct widget *prev = __rtk_curwidget;

	__rtk_curwidget = widget;
	lua_rawgeti(__rtk_L, LUA_REGISTRYINDEX, widget->ref); 
	lua_getglobal(__rtk_L, "get_attribute");
	lua_insert(__rtk_L, -3);
	lua_insert(__rtk_L, -3);
	if (lua_pcall(__rtk_L, 1, 1, 0)) {
#if DEBUG
		fprintf(stderr, "Toolkit Lua error: %s\n", lua_tostring(__rtk_L, -1));
#else
		lua_pop(__rtk_L, -1);
#endif
		mutex_free(&__rtk_mutex);
		__rtk_curwidget = prev;
		return 1;
	}
	__rtk_curwidget = prev;
	return 0;
}

#define ATTRIBUTE_FUNCS(ctype,typename,luatype) \
	int set_attribute_##typename(struct widget *widget, const char *name, ctype value) { \
		mutex_spin(&__rtk_mutex); \
		\
		lua_pushstring(__rtk_L, name); \
		lua_push##luatype(__rtk_L, value); \
		if (__rtk_set_attribute(widget)) { \
			mutex_free(&__rtk_mutex); \
			return 1; \
		} \
		\
		mutex_free(&__rtk_mutex); \
		return 0; \
	} \
	\
	ctype get_attribute_##typename(struct widget *widget, const char *name) { \
		ctype ret; \
		\
		mutex_spin(&__rtk_mutex); \
		\
		lua_pushstring(__rtk_L, name); \
		if (__rtk_get_attribute(widget)) { \
			mutex_free(&__rtk_mutex); \
			return 0; \
		} \
		if (!lua_is##luatype(__rtk_L, -1)) { \
			lua_pop(__rtk_L, -1); \
			mutex_free(&__rtk_mutex); \
			return 0; \
		} \
		ret = lua_to##luatype(__rtk_L, -1); \
		\
		mutex_free(&__rtk_mutex); \
		\
		return ret; \
	}

ATTRIBUTE_FUNCS(int, int, number)
ATTRIBUTE_FUNCS(double, double, number)
ATTRIBUTE_FUNCS(const char*, string, string)
