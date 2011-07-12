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
#include <lualib.h>

static lua_State *L;
static bool mutex;
static struct fb *fb;
static struct widget *curwidget;

int write_text(lua_State *L) {
	int x, y, size, fg, bg;
	const char *text;
	int ret = 0;

	if (!lua_isnumber(L, 1)) ret = 1;
	if (!lua_isnumber(L, 2)) ret = 1;
	if (!lua_isnumber(L, 3)) ret = 1;
	if (!lua_isstring(L, 4)) ret = 1;
	if (!lua_isnumber(L, 5)) ret = 1;
	if (!lua_isnumber(L, 6)) ret = 1;

	if (!ret) {
		x = lua_tonumber(L, 1);
		y = lua_tonumber(L, 2);
		size = lua_tonumber(L, 3);
		text = lua_tostring(L, 4);
		fg = lua_tonumber(L, 5);
		bg = lua_tonumber(L, 6);
	
		ret = fb_write(fb, curwidget->x + x, curwidget->y + y, size, text, strlen(text), fg, bg);
	}
	
	lua_pushboolean(L, ret);
	return 1;
}

int update(lua_State *L) {
	fb_flip(fb);
	return 0;
}

void init_toolkit(struct fb *fb_) {
	fb = fb_;
	L = lua_open();
	luaL_openlibs(L);

    lua_pushcfunction(L, write_text);
    lua_setglobal(L, "write_text");
    lua_pushcfunction(L, update);
    lua_setglobal(L, "update");
}

void close_toolkit() {
	lua_close(L);
}

struct widget *add_widget(const char *name, int x, int y, int w, int h) {
	struct widget *widget = malloc(sizeof(struct widget));
	char *filename;

	if (!widget) {
		return NULL;
	}

	widget->x = x;
	widget->y = y;

	mutex_spin(&mutex);

	filename = saprintf("/etc/%s.lua", name);
	if (luaL_loadfile(L, filename)) {
		free(widget);
		free(filename);
		mutex_free(&mutex);
		return NULL;
	}
	free(filename);

	lua_pushvalue(L,-1);
	widget->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pcall(L, 0, 0, 0);

	mutex_free(&mutex);

	set_size(widget, w, h);

	return widget;
}

void free_widget(struct widget *widget) {
	mutex_spin(&mutex);
	luaL_unref(L, LUA_REGISTRYINDEX, widget->ref);
	mutex_free(&mutex);
	free(widget);
}

int draw_widget(struct widget *widget) {
	mutex_spin(&mutex);

	curwidget = widget;

	lua_rawgeti(L, LUA_REGISTRYINDEX, widget->ref);
	lua_getglobal(L, "draw");
	if (lua_pcall(L, 0, 0, 0)) {
		lua_pop(L, -1);
		return 1;
	}

	curwidget = NULL;

	mutex_free(&mutex);

	return 0;
}

void set_position(struct widget *widget, int x, int y) {
	widget->x = x;
	widget->y = y;
}

int set_size(struct widget *widget, int width, int height) {
	widget->width = width;
	widget->height = height;

	return set_attribute_int(widget, "width", width) ||
	       set_attribute_int(widget, "height", height);
}

#define ATTRIBUTE_FUNCS(ctype,typename,luatype) \
	int set_attribute_##typename(struct widget *widget, const char *name, ctype value) { \
		mutex_spin(&mutex); \
		\
		lua_rawgeti(L, LUA_REGISTRYINDEX, widget->ref); \
		lua_getglobal(L, "setattribute"); \
		lua_pushstring(L, name); \
		lua_push##luatype(L, value); \
		if (lua_pcall(L, 2, 0, 0)) { \
			lua_pop(L, -1); \
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
		lua_rawgeti(L, LUA_REGISTRYINDEX, widget->ref); \
		lua_getglobal(L, "getattribute"); \
		lua_pushstring(L, name); \
		if (lua_pcall(L, 1, 1, 0)) { \
			mutex_free(&mutex); \
			return 0; \
		} \
		if (!lua_is##luatype(L, -1)) { \
			lua_pop(L, -1); \
			mutex_free(&mutex); \
			return 0; \
		} \
		ret = lua_to##luatype(L, -1); \
		\
		mutex_free(&mutex); \
		\
		return ret; \
	}


ATTRIBUTE_FUNCS(int, int, number)
ATTRIBUTE_FUNCS(double, double, number)
ATTRIBUTE_FUNCS(const char*, string, string)
