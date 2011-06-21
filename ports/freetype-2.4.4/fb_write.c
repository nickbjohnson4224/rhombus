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
#include <ft2build.h>
#include FT_FREETYPE_H

int fb_write(struct fb *fb, int x, int y, int height, const char *text, size_t length, uint32_t foreground, uint32_t background) {
	static FT_Library library;
	static FT_Face face;
	static bool init_done = false;
	uint32_t red, green, blue;
	double alpha;
	size_t i;
	int cursorx = 0;

	if (!init_done) {
		FT_Init_FreeType(&library);
		FT_New_Face(library, "/etc/font.ttf", 0, &face);
		init_done = true;
	}

	if (FT_Set_Pixel_Sizes(face, 0, height)) {
		return 1;
	}

	for (i = 0; i < length; i++) {
		if (FT_Load_Char(face, text[i], FT_LOAD_RENDER)) {
			continue;
		}
		FT_Bitmap *bitmap = &face->glyph->bitmap;
		for (int j = 0; j < bitmap->rows; j++) {
			for (int i = 0; i < bitmap->width; i++) {
				alpha = bitmap->buffer[j * bitmap->width + i] / 255.0;
				red   = alpha * PIX_R(foreground) + (1 - alpha) * PIX_R(background);
				green = alpha * PIX_G(foreground) + (1 - alpha) * PIX_G(background);
				blue  = alpha * PIX_B(foreground) + (1 - alpha) * PIX_B(background);
				fb_plot(fb, x + cursorx + i , y + j + (height - bitmap->rows), COLOR(red, green, blue));
			}
		}
		cursorx += face->glyph->advance.x >> 6;
	}

	return 0;
}
