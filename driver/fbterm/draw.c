/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repän <jrepat at gmail.com>
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

#include "fbterm.h"

#include <stdlib.h>
#include <graph.h>
#include <stdio.h>

struct cache {
	uint32_t *bitmap;
	uint32_t ch;
	uint32_t fg, bg;
	struct cache *prev, *next;
};

const size_t max_cache_size = 100;

struct fb *fb = NULL;
struct cache *cache, *last;
size_t cache_size;

int draw_cell(struct cell *c, int x, int y) {
	FT_Bitmap *bitmap;
	struct cache *ptr;
	uint32_t red, green, blue;
	double alpha;

	for (ptr = cache; ptr; ptr = ptr->next) {
		if (c->ch == ptr->ch && c->fg == ptr->fg && c->bg == ptr->bg) {
			if (ptr != cache) {
				// Most recent bitmaps are kept in the beginning of the cache
				if (ptr == last) {
					last = ptr->prev;
				}
				ptr->prev->next = ptr->next;
				if (ptr->next) {
					ptr->next->prev = ptr->prev;
				}
				ptr->prev = NULL;
				ptr->next = cache;
				cache->prev = ptr;
				cache = ptr;
			}
			return fb_blit(fb, ptr->bitmap, x, y, screen.cell_width, screen.cell_height);
		}
	}

	if (FT_Load_Char(face, c->ch, FT_LOAD_RENDER)) {
		return 1;
	}

	ptr = malloc(sizeof(struct cache));
	if (!ptr) {
		return 1;
	}
	ptr->bitmap = malloc(screen.cell_width * screen.cell_height * sizeof(uint32_t));
	if (!ptr->bitmap) {
		free(ptr);
		return 1;
	}
	ptr->ch = c->ch;
	ptr->fg = c->fg;
	ptr->bg = c->bg;
	ptr->prev = NULL;
	ptr->next = cache;
	if (cache) {
		cache->prev = ptr;
	}
	cache = ptr;
	if (!last) {
		last = ptr;
	}
	cache_size++;
	if (cache_size > max_cache_size) {
		// Remove the least recent bitmap
		last = last->prev;
		free(last->next->bitmap);
		free(last->next);
		last->next = NULL;
		cache_size--;
	}

	bitmap = &face->glyph->bitmap;
	for (int j = 0; j < screen.cell_height; j++) {
		for (int i = 0; i < screen.cell_width; i++) {
			ptr->bitmap[i + j * screen.cell_width] = c->bg;
		}
	}
	for (int j = 0; j < bitmap->rows; j++) {
		for (int i = 0; i < bitmap->width; i++) {
			alpha = bitmap->buffer[j * bitmap->width + i] / 255.0;
			red   = alpha * PIX_R(c->fg) + (1 - alpha) * PIX_R(c->bg);
			green = alpha * PIX_G(c->fg) + (1 - alpha) * PIX_G(c->bg);
			blue  = alpha * PIX_B(c->fg) + (1 - alpha) * PIX_B(c->bg);
			ptr->bitmap[i + face->glyph->bitmap_left +
				(j + screen.font_size - face->glyph->bitmap_top) * screen.cell_width] =
				COLOR(red, green, blue);
		}
	}

	return fb_blit(fb, ptr->bitmap, x, y, screen.cell_width, screen.cell_height);
}

