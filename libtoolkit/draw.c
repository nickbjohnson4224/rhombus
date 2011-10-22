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

#include <rho/graph.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <lua.h>
#include "private.h"

struct font {
	FT_Face face;
	bool mutex;
	struct font *prev, *next;
};

static FT_Library library;
static struct font default_font;
static bool default_font_ready;

int __rtk_init_freetype() {
	return FT_Init_FreeType(&library);
}

int __rtk_set_default_font(const char *path) {
	if (default_font_ready) {
		FT_Done_Face(default_font.face);
	}

	if (FT_New_Face(library, path, 0, &default_font.face)) {
		return 1;
	}

	default_font_ready = true;
	return 0;
}

void __rtk_free_font(struct font *font) {
	if (font->next) {
		__rtk_free_font(font->next);
	}
	FT_Done_Face(font->face);
	free(font);
}

static int plot_pixel(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	int ret = 0;
	int x, y;
	uint32_t color;

	if (lua_gettop(L) < 3) ret = 1;
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

	if (lua_gettop(L) < 5) ret = 1;
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
			ret = fb_fill(widget->window->fb, widget->realx + x, widget->realy + y, width, height, color);
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

	if (lua_gettop(L) < 5) ret = 1;
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
			bitmap[i++] = lua_tonumber(L, -1);
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

static int load_font(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	struct font *font;
	const char *name;

	name = lua_tostring(L, 1);
	if (!name) {
		lua_pushnil(L);
		return 1;
	}

	font = malloc(sizeof(struct font));
	if (!font) {
		lua_pushnil(L);
		return 1;
	}
	font->mutex = false;

	if (FT_New_Face(library, name, 0, &font->face)) {
		free(font);
		lua_pushnil(L);
		return 1;
	}

	font->prev = NULL;
	if (widget->fonts) {
		widget->fonts->prev = font;
	}
	font->next = widget->fonts;
	widget->fonts = font;
	lua_pushlightuserdata(L, font);
	return 1;
}

static int free_font(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	struct font *font = lua_touserdata(L, 1);

	if (!font) {
		return 0;
	}

	if (font->next) {
		font->next->prev = font->prev;
	}
	if (font->prev) {
		font->prev->next = font->next;
	}
	if (widget->fonts == font) {
		widget->fonts = font->next;
	}

	FT_Done_Face(font->face);
	free(font);
	return 0;
}

static int write_text(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	struct font *font;
	FT_Bitmap *bitmap;
	const char *text;
	uint32_t foreground, background;
	uint32_t red, green, blue;
	double alpha;
	int x, y, size;
	int ret = 0;
	int advance = 0;
	int cursorx, cursory;

	if (lua_gettop(L) < 7) ret = 1;
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
		font = lua_touserdata(L, 7);

		mutex_spin(&font->mutex);

		if (FT_Set_Pixel_Sizes(font->face, 0, size)) {
			ret = 1;
		}
		else {
			for (size_t c = 0; c < strlen(text) && advance < widget->realwidth; c++) {
				if (FT_Load_Char(font->face, text[c], FT_LOAD_RENDER)) {
					continue;
				}
				bitmap = &font->face->glyph->bitmap;
				cursory = y + (size - font->face->glyph->bitmap_top);
				for (int j = 0; j < bitmap->rows && cursory < widget->realheight; j++, cursory++) {
					cursorx = x + advance + font->face->glyph->bitmap_left;
					for (int i = 0; i < bitmap->width && cursorx < widget->realwidth; i++, cursorx++) {
						alpha = bitmap->buffer[j * bitmap->width + i] / 255.0;
						red   = alpha * PIX_R(foreground) + (1 - alpha) * PIX_R(background);
						green = alpha * PIX_G(foreground) + (1 - alpha) * PIX_G(background);
						blue  = alpha * PIX_B(foreground) + (1 - alpha) * PIX_B(background);
						fb_plot(widget->window->fb, cursorx + widget->realx,
								cursory + widget->realy, COLOR(red, green, blue));
					}
				}
				advance += font->face->glyph->advance.x >> 6;
			}
		}

		mutex_free(&font->mutex);
	}
	
	lua_pushboolean(L, ret);
	return 1;
}

void __rtk_init_drawing_functions(lua_State *L) {
	lua_pushlightuserdata(L, &default_font);
	lua_setglobal(L, "default_font");

	EXPORT_FUNC(plot_pixel);
	EXPORT_FUNC(fill);
	EXPORT_FUNC(blit);

	EXPORT_FUNC(load_font);
	EXPORT_FUNC(free_font);
	EXPORT_FUNC(write_text);
}
