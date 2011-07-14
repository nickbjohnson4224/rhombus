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

static void init_freetype() {
	FT_Init_FreeType(&library);
	FT_New_Face(library, "/etc/dejavu.ttf", 0, &face); //todo: configure font
}

static int update(lua_State *__rtk_L) {
	fb_flip(__rtk_curwidget->window->fb);
	return 0;
}

static int write_text(lua_State *__rtk_L) {
	FT_Bitmap *bitmap;
	const char *text;
	uint32_t red, green, blue;
	double alpha;
	int x, y, size, foreground, background;
	int ret = 0;
	int advance = 0;
	int cursorx, cursory;

	if (!lua_isnumber(__rtk_L, 1)) ret = 1;
	if (!lua_isnumber(__rtk_L, 2)) ret = 1;
	if (!lua_isnumber(__rtk_L, 3)) ret = 1;
	if (!lua_isstring(__rtk_L, 4)) ret = 1;
	if (!lua_isnumber(__rtk_L, 5)) ret = 1;
	if (!lua_isnumber(__rtk_L, 6)) ret = 1;

	if (!ret) {
		x = lua_tonumber(__rtk_L, 1);
		y = lua_tonumber(__rtk_L, 2);
		size = lua_tonumber(__rtk_L, 3);
		text = lua_tostring(__rtk_L, 4);
		foreground = lua_tonumber(__rtk_L, 5);
		background = lua_tonumber(__rtk_L, 6);

		if (FT_Set_Pixel_Sizes(face, 0, size)) {
			ret = 1;
		}
		else {
			for (size_t c = 0; c < strlen(text) && advance < __rtk_curwidget->width; c++) {
				if (FT_Load_Char(face, text[c], FT_LOAD_RENDER)) {
					continue;
				}
				bitmap = &face->glyph->bitmap;
				cursory = y + (size - face->glyph->bitmap_top);
				for (int j = 0; j < bitmap->rows && cursory < __rtk_curwidget->height; j++, cursory++) {
					cursorx = x + advance + face->glyph->bitmap_left;
					for (int i = 0; i < bitmap->width && cursorx < __rtk_curwidget->width; i++, cursorx++) {
						alpha = bitmap->buffer[j * bitmap->width + i] / 255.0;
						red   = alpha * PIX_R(foreground) + (1 - alpha) * PIX_R(background);
						green = alpha * PIX_G(foreground) + (1 - alpha) * PIX_G(background);
						blue  = alpha * PIX_B(foreground) + (1 - alpha) * PIX_B(background);
						fb_plot(__rtk_curwidget->window->fb, __rtk_curwidget->x + cursorx, __rtk_curwidget->y + cursory,
								COLOR(red, green, blue));
					}
				}
				advance += face->glyph->advance.x >> 6;
			}
		}
	}
	
	lua_pushboolean(__rtk_L, ret);
	return 1;
}

#define EXPORT_FUNC(x) \
    lua_pushcfunction(__rtk_L, x); \
    lua_setglobal(__rtk_L, #x);

void __rtk_init_drawing_functions() {
	init_freetype();
	EXPORT_FUNC(update);
	EXPORT_FUNC(write_text);
}
