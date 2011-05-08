/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

struct fb *fb = NULL;

int draw_cell(struct font *font, struct cell *c, int x, int y) {
	struct glyph *glyph;
	uint32_t *bitmap;
	int i;

	if (!font || !c) {
		return 1;
	}

	if (c->ch >= font->count) {
		glyph = font->def_glyph;
	}
	else {
		glyph = font->glyph[c->ch];

		if (!glyph) {
			glyph = font->def_glyph;
			if (!glyph) {
				return 1;
			}
		}
	}

	// construct bitmap
	bitmap = malloc(sizeof(uint32_t) * glyph->w * glyph->h);
	for (i = 0; i < glyph->w * glyph->h; i++) {
		bitmap[i] = (glyph->value[i]) ? c->fg : c->bg;
	}

	// blit onto framebuffer
	fb_blit(fb, bitmap, x, y, glyph->w, glyph->h);

	return 0;
}
