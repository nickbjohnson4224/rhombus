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

#include <assert.h>
#include <graph.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <lua.h>
#include <lauxlib.h>
#include "widget.h"
#include "window.h"
#include "private.h"

static FT_Library library;
static FT_Face face;
static bool mutex;

int __rtk_init_freetype() {
	return FT_Init_FreeType(&library) ||
	       FT_New_Face(library, "/etc/dejavu.ttf", 0, &face); //todo: configure font
}

struct widget *get_widget(lua_State *L) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
		return lua_touserdata(L, -1);
}

void mark_child_dirty(struct widget *widget) {
	widget->child_dirty = true;
	if (widget->parent) {
		mark_child_dirty(widget->parent);
	}
}
static int request_redraw(lua_State *L) {
	struct widget *widget = get_widget(L);

	widget->dirty = true;
	if (widget->parent) {
		mark_child_dirty(widget->parent);
	}
	return 0;
}

static int send_event(lua_State *L) {
	struct widget *widget = get_widget(L);

	if (lua_isstring(L, 1) && widget->window->handler) {
		widget->window->handler(widget, lua_tostring(L, 1));
	}
	return 0;
}

static int add_child(lua_State *L) {
	bool error = false;
	struct widget *child = NULL;
	struct widget *widget = get_widget(L);
	const char *type;
	int x, y, width, height;

	if (!lua_isstring(L, 1)) error = true;
	if (!lua_isnumber(L, 2)) error = true;
	if (!lua_isnumber(L, 3)) error = true;
	if (!lua_isnumber(L, 4)) error = true;
	if (!lua_isnumber(L, 5)) error = true;

	if (!error) {
		type = lua_tostring(L, 1);
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
		width = lua_tonumber(L, 4);
		height = lua_tonumber(L, 5);

		child = add_widget(type, widget, widget->window, x, y, width, height);
	}

	lua_pushlightuserdata(L, child);
	return 1;
}

static int call_child(lua_State *L) {
	struct widget *child = lua_touserdata(L, 1);
	const char *func = lua_tostring(L, 2);
	const char *arg;

	lua_getglobal(child->L, func);

	for (int i = 3; i <= lua_gettop(L); i++) {
		arg = lua_tostring(L, i);
		lua_pushstring(child->L, arg);
	}

	int ret = __rtk_call_lua_function(child->L, lua_gettop(L) - 2, 0);
	lua_pushboolean(L, ret);
	return 1;
}

static int set_child_attribute(lua_State *L) {
	struct widget *child = lua_touserdata(L, 1);
	const char *name, *data;
	int value = 0;
	int ret = 0;

	if (!lua_isstring(L, 2)) {
		lua_pushnumber(L, 0);
		return 1;
	}

	name = lua_tostring(L, 2);
	data = lua_tostring(L, 3);
	if (lua_isnumber(L, 3)) {
		value = lua_tonumber(L, 3);
	}
	
	if (!strcmp(name, "x")) {
		child->x = value;
		update_widget(child);
	}
	else if (!strcmp(name, "y")) {
		child->y = value;
		update_widget(child);
	}
	else {
		lua_pushstring(child->L, name);
		lua_pushstring(child->L, data);
		ret = __rtk_set_attribute(child);
	}

	if (!ret) {
		if (!strcmp(name, "width")) {
			child->width = value;
			update_widget(child);
		}
		if (!strcmp(name, "height")) {
			child->height = value;
			update_widget(child);
		}
	}

	lua_pushnumber(L, ret);
	return 1;
}

static int get_child_attribute(lua_State *L) {
	struct widget *child = lua_touserdata(L, 1);
	const char *name, *value;

	if (!lua_isstring(L, 2)) {
		lua_pushnumber(L, 0);
		return 1;
	}
	name = lua_tostring(L, 2);

	if (!strcmp(name, "x")) {
		lua_pushnumber(L, child->x);
	}
	else if (!strcmp(name, "y")) {
		lua_pushnumber(L, child->y);
	}
	else if (!strcmp(name, "width")) {
		lua_pushnumber(L, child->width);
	}
	else if (!strcmp(name, "height")) {
		lua_pushnumber(L, child->height);
	}
	else {
		lua_pushstring(child->L, name);
		if (__rtk_get_attribute(child)) {
			lua_pushnumber(L, 0);
		}
		else {
			value = lua_tostring(child->L, -1);
			lua_pushstring(L, value);
		}
	}
	return 1;
}

static int write_text(lua_State *L) {
	struct widget *widget = get_widget(L);
	FT_Bitmap *bitmap;
	const char *text;
	uint32_t red, green, blue;
	double alpha;
	int x, y, size, foreground, background;
	int ret = 0;
	int advance = 0;
	int cursorx, cursory;

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
		foreground = lua_tonumber(L, 5);
		background = lua_tonumber(L, 6);

		foreground = COLOR_WHITE; //fixme: default value

		mutex_spin(&mutex);

		if (FT_Set_Pixel_Sizes(face, 0, size)) {
			ret = 1;
		}
		else {
			for (size_t c = 0; c < strlen(text) && advance < widget->realwidth; c++) {
				if (FT_Load_Char(face, text[c], FT_LOAD_RENDER)) {
					continue;
				}
				bitmap = &face->glyph->bitmap;
				cursory = y + (size - face->glyph->bitmap_top);
				for (int j = 0; j < bitmap->rows && cursory < widget->realheight; j++, cursory++) {
					cursorx = x + advance + face->glyph->bitmap_left;
					for (int i = 0; i < bitmap->width && cursorx < widget->realwidth; i++, cursorx++) {
						alpha = bitmap->buffer[j * bitmap->width + i] / 255.0;
						red   = alpha * PIX_R(foreground) + (1 - alpha) * PIX_R(background);
						green = alpha * PIX_G(foreground) + (1 - alpha) * PIX_G(background);
						blue  = alpha * PIX_B(foreground) + (1 - alpha) * PIX_B(background);
						fb_plot(widget->window->fb, cursorx + widget->realx,
								cursory + widget->realy, COLOR(red, green, blue));
					}
				}
				advance += face->glyph->advance.x >> 6;
			}
		}

		mutex_free(&mutex);
	}
	
	lua_pushboolean(L, ret);
	return 1;
}

#define EXPORT_FUNC(x) \
    lua_pushcfunction(L, x); \
    lua_setglobal(L, #x);

void __rtk_init_drawing_functions(lua_State *L) {
	EXPORT_FUNC(request_redraw);
	EXPORT_FUNC(send_event);

	EXPORT_FUNC(add_child);
	EXPORT_FUNC(call_child);
	EXPORT_FUNC(set_child_attribute);
	EXPORT_FUNC(get_child_attribute);

	EXPORT_FUNC(write_text);
}
