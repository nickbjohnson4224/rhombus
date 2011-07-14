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

struct widget *__rtk_curwidget;
static FT_Library library;
static FT_Face face;
int __rtk_curx, __rtk_cury, __rtk_curwidth, __rtk_curheight;

static void init_freetype() {
	FT_Init_FreeType(&library);
	FT_New_Face(library, "/etc/dejavu.ttf", 0, &face); //todo: configure font
}

void mark_child_dirty(struct widget *widget) {
	widget->child_dirty = true;
	if (widget->parent) {
		mark_child_dirty(widget->parent);
	}
}
static int request_redraw(lua_State *L) {
	__rtk_curwidget->dirty = true;
	if (__rtk_curwidget->parent) {
		mark_child_dirty(__rtk_curwidget->parent);
	}
	return 0;
}

static int add_child(lua_State *L) {
	bool error = false;
	struct widget *widget = NULL;
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
		width = lua_tonumber(L, 2);
		height = lua_tonumber(L, 3);

		mutex_free(&__rtk_mutex);
		widget = add_widget(type, __rtk_curwidget, __rtk_curwidget->window, x, y, width, height);
		assert(mutex_lock(&__rtk_mutex));
	}

	lua_pushlightuserdata(L, widget);
	return 1;
}

static int set_child_attribute(lua_State *L) {
	struct widget *widget = lua_touserdata(L, 1);
	const char *name;
	int value = 0;
	int ret = 0;

	if (!lua_isstring(L, 2)) {
		lua_pushnumber(L, 0);
		return 1;
	}
	name = lua_tostring(L, 2);
	if (lua_isnumber(L, 3)) {
		value = lua_tonumber(L, 3);
	}
	
	if (!strcmp(name, "x")) {
		widget->x = value;
	}
	else if (!strcmp(name, "y")) {
		widget->y = value;
	}
	else {
		ret = __rtk_set_attribute(widget);
	}

	if (!ret) {
		if (!strcmp(name, "width")) {
			widget->width = value;
		}
		if (!strcmp(name, "height")) {
			widget->height = value;
		}
	}

	lua_pushnumber(L, ret);
	return 1;
}

static int get_child_attribute(lua_State *L) {
	struct widget *widget = lua_touserdata(L, 1);
	const char *name;

	if (!lua_isstring(L, 2)) {
		lua_pushnumber(L, 0);
		return 1;
	}
	name = lua_tostring(L, 2);

	if (!strcmp(name, "x")) {
		lua_pushnumber(L, widget->x);
	}
	else if (!strcmp(name, "y")) {
		lua_pushnumber(L, widget->y);
	}
	else if (!strcmp(name, "width")) {
		lua_pushnumber(L, widget->width);
	}
	else if (!strcmp(name, "height")) {
		lua_pushnumber(L, widget->height);
	}
	else if (__rtk_get_attribute(widget)) {
		lua_pushnumber(L, 0);
	}
	return 1;
}

static int write_text(lua_State *L) {
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

		if (FT_Set_Pixel_Sizes(face, 0, size)) {
			ret = 1;
		}
		else {
			for (size_t c = 0; c < strlen(text) && advance < __rtk_curwidth; c++) {
				if (FT_Load_Char(face, text[c], FT_LOAD_RENDER)) {
					continue;
				}
				bitmap = &face->glyph->bitmap;
				cursory = y + (size - face->glyph->bitmap_top);
				for (int j = 0; j < bitmap->rows && cursory < __rtk_curheight; j++, cursory++) {
					cursorx = x + advance + face->glyph->bitmap_left;
					for (int i = 0; i < bitmap->width && cursorx < __rtk_curwidth; i++, cursorx++) {
						alpha = bitmap->buffer[j * bitmap->width + i] / 255.0;
						red   = alpha * PIX_R(foreground) + (1 - alpha) * PIX_R(background);
						green = alpha * PIX_G(foreground) + (1 - alpha) * PIX_G(background);
						blue  = alpha * PIX_B(foreground) + (1 - alpha) * PIX_B(background);
						fb_plot(__rtk_curwidget->window->fb, __rtk_curwidget->x + cursorx + __rtk_curx,
								__rtk_curwidget->y + cursory + __rtk_cury, COLOR(red, green, blue));
					}
				}
				advance += face->glyph->advance.x >> 6;
			}
		}
	}
	
	lua_pushboolean(L, ret);
	return 1;
}

#define EXPORT_FUNC(x) \
    lua_pushcfunction(__rtk_L, x); \
    lua_setglobal(__rtk_L, #x);

void __rtk_init_drawing_functions() {
	init_freetype();

	EXPORT_FUNC(request_redraw);

	EXPORT_FUNC(add_child);
	EXPORT_FUNC(set_child_attribute);
	EXPORT_FUNC(get_child_attribute);

	EXPORT_FUNC(write_text);
}
