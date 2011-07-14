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

static bool mutex;

struct widget *add_widget(const char *name, struct window *window, int x, int y, int w, int h) {
	struct widget *widget = malloc(sizeof(struct widget));
	char *filename;

	if (!widget) {
		return NULL;
	}

	widget->window = window;
	widget->x = x;
	widget->y = y;

	mutex_spin(&mutex);

	filename = saprintf("/etc/widgets/%s.lua", name);
	if (luaL_loadfile(__rtk_L, filename)) {
		free(widget);
		free(filename);
		mutex_free(&mutex);
		return NULL;
	}
	free(filename);

	lua_pushvalue(__rtk_L,-1);
	widget->ref = luaL_ref(__rtk_L, LUA_REGISTRYINDEX);
	lua_pcall(__rtk_L, 0, 0, 0);

	mutex_free(&mutex);

	set_size(widget, w, h);

	return widget;
}

void free_widget(struct widget *widget) {
	mutex_spin(&mutex);
	luaL_unref(__rtk_L, LUA_REGISTRYINDEX, widget->ref);
	mutex_free(&mutex);
	free(widget);
}

int draw_widget(struct widget *widget) {
	mutex_spin(&mutex);

	__rtk_curwidget = widget;

	lua_rawgeti(__rtk_L, LUA_REGISTRYINDEX, widget->ref);
	lua_getglobal(__rtk_L, "draw");
	if (lua_pcall(__rtk_L, 0, 0, 0)) {
		lua_pop(__rtk_L, -1);
		return 1;
	}

	__rtk_curwidget = NULL;

	mutex_free(&mutex);

	return 0;
}

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

#define ATTRIBUTE_FUNCS(ctype,typename,luatype) \
	int set_attribute_##typename(struct widget *widget, const char *name, ctype value) { \
		mutex_spin(&mutex); \
		\
		lua_rawgeti(__rtk_L, LUA_REGISTRYINDEX, widget->ref); \
		lua_getglobal(__rtk_L, "set_attribute"); \
		lua_pushstring(__rtk_L, name); \
		lua_push##luatype(__rtk_L, value); \
		if (lua_pcall(__rtk_L, 2, 0, 0)) { \
			lua_pop(__rtk_L, -1); \
			mutex_free(&mutex); \
			return 1; \
		} \
		\
		mutex_free(&mutex); \
		return 0; \
	} \
	\
	ctype get_attribute_##typename(struct widget *widget, const char *name) { \
		ctype ret; \
		\
		mutex_spin(&mutex); \
		\
		lua_rawgeti(__rtk_L, LUA_REGISTRYINDEX, widget->ref); \
		lua_getglobal(__rtk_L, "get_attribute"); \
		lua_pushstring(__rtk_L, name); \
		if (lua_pcall(__rtk_L, 1, 1, 0)) { \
			mutex_free(&mutex); \
			return 0; \
		} \
		if (!lua_is##luatype(__rtk_L, -1)) { \
			lua_pop(__rtk_L, -1); \
			mutex_free(&mutex); \
			return 0; \
		} \
		ret = lua_to##luatype(__rtk_L, -1); \
		\
		mutex_free(&mutex); \
		\
		return ret; \
	}


ATTRIBUTE_FUNCS(int, int, number)
ATTRIBUTE_FUNCS(double, double, number)
ATTRIBUTE_FUNCS(const char*, string, string)
