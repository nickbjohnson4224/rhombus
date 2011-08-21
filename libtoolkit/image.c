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

//requirements for BMP: BITMAPINFOHEADER; 32bbp RGBA

#include <assert.h>
#include <graph.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include "private.h"

struct file_header {
	uint16_t signature;
	uint32_t file_size;
	uint32_t reserved;
	uint32_t pixel_array_offset;
} __attribute__((packed));

struct dib_header {
	uint32_t header_size;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
	int32_t horizontal_resolution;
	int32_t vertical_resolution;
	uint32_t colors;
	uint32_t important_colors;
} __attribute__((packed));

struct image {
	int width, height;
	uint32_t *top_left, *top_right, *bottom_left, *bottom_right;
	uint32_t *top, *bottom, *left, *right;
};


static uint32_t *get_bitmap(struct dib_header dib_header, uint32_t *pixels, int x, int y, int width, int height) {
//todo: dib_header pointerina??
	uint32_t *ret = malloc(width * height * sizeof(uint32_t));
	assert(ret);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			ret[i * width + j] = pixels[(dib_header.height - j - 1) * dib_header.width + i];
		}
	}
	return ret;
}

static struct image *__load_image(const char *filename) {
	struct file_header file_header;
	struct dib_header dib_header;
	uint32_t *pixels;
	FILE *file;
	struct image *image;

	file = fopen(filename, "r");
	if (!file) {
		return NULL;
	}

	fread(&file_header, sizeof(file_header), 1, file);
	if (file_header.signature != 0x4d42) {
		fclose(file);
		return NULL;
	}

	fread(&dib_header, sizeof(dib_header), 1, file);
	if (dib_header.header_size != sizeof(dib_header) || dib_header.compression != 0 || dib_header.bits_per_pixel != 32) {
		fclose(file);
		return NULL;
	}

	fseek(file, file_header.pixel_array_offset, SEEK_SET);
	pixels = malloc(dib_header.image_size);
	if (!pixels) {
		fclose(file);
		return NULL;
	}
	fread(pixels, dib_header.image_size, 1, file);

	fclose(file);

	image = malloc(sizeof(image));
	assert(image);
	image->width  = dib_header.width;
	image->height = dib_header.height;

	image->top_left		= get_bitmap(dib_header, pixels, 0, 0,
			dib_header.width / 2, dib_header.height / 2);
	image->top_right	= get_bitmap(dib_header, pixels, dib_header.width / 2, 0,
			dib_header.width / 2, dib_header.height / 2);
	image->bottom_left	= get_bitmap(dib_header, pixels, 0, dib_header.height / 2,
			dib_header.width / 2, dib_header.height / 2);
	image->bottom_right	= get_bitmap(dib_header, pixels, dib_header.width / 2, dib_header.height / 2,
			dib_header.width / 2, dib_header.height / 2);
	image->top		= get_bitmap(dib_header, pixels, dib_header.width / 2, 0,
			1, dib_header.height / 2);
	image->bottom	= get_bitmap(dib_header, pixels, dib_header.width / 2, dib_header.height / 2,
			1, dib_header.height / 2);
	image->left		= get_bitmap(dib_header, pixels, 0, dib_header.height / 2,
			dib_header.width / 2, 1);
	image->right	= get_bitmap(dib_header, pixels, dib_header.width / 2, dib_header.height / 2,
			dib_header.width / 2, 1);

	free(pixels);

	return image;
}

static int load_image(lua_State *L) {
	const char *filename;
	struct image *image;

	filename = lua_tostring(L, 1);
	if (!filename) {
		lua_pushnil(L);
		return 1;
	}

	image = __load_image(filename);

	if (!image) {
		lua_pushnil(L);
	}
	else {
		lua_pushlightuserdata(L, image);
	}
	return 1;
}

static int draw_image(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);
	struct image *image;
	int x, y, width, height;
	bool error = false;

	image = lua_touserdata(L, 1);

	if (!image) error = true;
	if (!lua_isnumber(L, 2)) error = true;
	if (!lua_isnumber(L, 3)) error = true;
	if (!lua_isnumber(L, 4)) error = true;
	if (!lua_isnumber(L, 5)) error = true;

	if (!error) {
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
		width = lua_tonumber(L, 4);
		height = lua_tonumber(L, 5);

		if (width < image->width || height < image->height) {
			error = true;
		}
		else {
			error |= fb_blit(widget->window->fb, image->top_left,
					widget->realx + x, widget->realy + y, image->width / 2, image->height / 2);
			error |= fb_blit(widget->window->fb, image->top_right, widget->realx + x
					+ width - image->width / 2, widget->realy + y, image->width / 2, image->height / 2);
			error |= fb_blit(widget->window->fb, image->bottom_left, widget->realx + x,
					widget->realy + y + height - image->height / 2, image->width / 2, image->height / 2);
			error |= fb_blit(widget->window->fb, image->bottom_right,
					widget->realx + x + width - image->width / 2,
					widget->realy + y + height - image->height / 2, image->width / 2, image->height / 2);

			for (int i = 0; i < width - image->width; i++) {
				error |= fb_blit(widget->window->fb, image->top,
						widget->realx + x + image->width / 2 + i, widget->realy + y, 1, image->height / 2);
				error |= fb_blit(widget->window->fb, image->bottom, widget->realx + x + image->width / 2 + i,
						widget->realy + y + height - image->height / 2, 1, image->height / 2);
			}

			for (int i = 0; i < height - image->height; i++) {
				error |= fb_blit(widget->window->fb, image->left, widget->realx + x,
						widget->realy + y + image->height / 2 + i, image->width / 2, 1);
				error |= fb_blit(widget->window->fb, image->right, widget->realx + x + width - image->width / 2,
						widget->realy + y + image->height / 2 + i, image->width / 2, 1);
			}
		}
	}

	lua_pushboolean(L, error);
	return 1;
}

static void __free_image(struct image *image) {
	free(image->top_left);
	free(image->top_right);
	free(image->bottom_left);
	free(image->bottom_right);
	free(image->top);
	free(image->bottom);
	free(image->left);
	free(image->right);
	free(image);
}

static int free_image(lua_State *L) { //todo: widgeti surma ajal automaatne free ka
	struct image *image;

	image = lua_touserdata(L, 1);
	if (image) {
		__free_image(image);
	}

	return 0;
}

static int get_width(lua_State *L) {
	struct image *image;

	image = lua_touserdata(L, 1);
	if (!image) {
		lua_pushnil(L);
	}
	else {
		lua_pushnumber(L, image->width);
	}

	return 1;
}

static int get_height(lua_State *L) {
	struct image *image;

	image = lua_touserdata(L, 1);
	if (!image) {
		lua_pushnil(L);
	}
	else {
		lua_pushnumber(L, image->height);
	}

	return 1;
}

void __rtk_init_image_functions(lua_State *L) {
	EXPORT_FUNC(load_image);
	EXPORT_FUNC(draw_image);
	EXPORT_FUNC(free_image);
	EXPORT_FUNC(get_width);
	EXPORT_FUNC(get_height);
//todo	EXPORT_FUNC(get_bitmap);
}
