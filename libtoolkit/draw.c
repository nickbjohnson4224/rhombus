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
#include "private.h"

static FT_Library library;
static FT_Face face;
static bool mutex;

int __rtk_init_freetype() {
	return FT_Init_FreeType(&library) ||
	       FT_New_Face(library, "/etc/dejavu.ttf", 0, &face); //todo: configure font
}

static int plot_pixel(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	int ret = 0;
	int x, y;
	uint32_t color;

	if (!lua_isnumber(L, 1)) ret = 1;
	if (!lua_isnumber(L, 2)) ret = 1;
	if (!lua_isnumber(L, 3)) ret = 1;

	if (!ret) {
		x = lua_tonumber(L, 1);
		y = lua_tonumber(L, 2);
		color = lua_tonumber(L, 3);

		if (x > widget->realwidth || y > widget->realheight) {
			ret = 1;
		}
		else {
			ret = fb_plot(widget->window->fb, widget->realx + x, widget->realy + y, color);
		}
	}

	lua_pushboolean(L, ret);
	return 1;
}

static int fill(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	uint32_t color;
	int ret = 0;
	int x, y, width, height;
	int i, j;

	if (!lua_isnumber(L, 1)) ret = 1;
	if (!lua_isnumber(L, 2)) ret = 1;
	if (!lua_isnumber(L, 3)) ret = 1;
	if (!lua_isnumber(L, 4)) ret = 1;
	if (!lua_isnumber(L, 5)) ret = 1;

	if (!ret) {
		x = lua_tonumber(L, 1);
		y = lua_tonumber(L, 2);
		width = lua_tonumber(L, 3);
		height = lua_tonumber(L, 4);
		color = lua_tonumber(L, 5);

		if (width < 0 || height < 0) {
			ret = 1;
		}
		else {
			for (i = 0; i < height && i < widget->realheight; i++) {
				for (j = 0; j < width && j < widget->realwidth; j++) {
					ret |= fb_plot(widget->window->fb, widget->realx + x + j, widget->realy + y + i, color);
				}
			}
		}
	}

	lua_pushboolean(L, ret);
	return 1;
}

static int blit(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	uint32_t *bitmap;
	int ret = 0;
	int x, y, width, height;
	int i = 0;

	if (!lua_isnumber(L, 2)) ret = 1;
	if (!lua_isnumber(L, 3)) ret = 1;
	if (!lua_isnumber(L, 4)) ret = 1;
	if (!lua_isnumber(L, 5)) ret = 1;

	if (!ret) {
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
		width = lua_tonumber(L, 4);
		height = lua_tonumber(L, 5);
		bitmap = malloc(width * height * sizeof(uint32_t));

		lua_pushnil(L);
		while (lua_next(L, 1) != 0) {
			bitmap[i++] = lua_tointeger(L, -1);
			lua_pop(L, 1);
		}

		if (width < 0 || height < 0) {
			ret = 1;
		}
		else {
			ret = fb_blit(widget->window->fb, bitmap, widget->realx + x, widget->realy + y, width, height);
		}

		free(bitmap);
	}

	lua_pushboolean(L, ret);
	return 1;
}

static int write_text(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
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

void __rtk_init_drawing_functions(lua_State *L) {
	EXPORT_FUNC(plot_pixel);
	EXPORT_FUNC(fill);
	EXPORT_FUNC(blit);

	EXPORT_FUNC(write_text);
}
