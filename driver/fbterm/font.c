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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define STR_SPC 	"                                                            "
#define STR_BANG	"        #     #     #     #     #           #               "
#define STR_DQUOTE	"       # #   # #   # #                                      "

static struct glyph *make_glyph(int w, int h, const char *str) {
	struct glyph *glyph;
	int i;

	glyph = malloc(sizeof(glyph) + sizeof(uint8_t) * w * h);
	glyph->w = w;
	glyph->h = h;
	
	for (i = 0; str[i]; i++) {
		if (str[i] == '#') {
			glyph->value[i] = 0xFF;
		}
		else {
			glyph->value[i] = 0x00;
		}
	}

	return glyph;
}

struct font *font_load(const char *path) {
	struct font *font;
	
	if (!strcmp(path, "builtin")) {
		font = malloc(sizeof(struct font) + sizeof(struct glyph*) * 128);
		font->w = 6;
		font->h = 8;
		font->count = 128;
		font->def_glyph = make_glyph(6, 8, STR_SPC);

		font->glyph[0x00] = NULL;
		font->glyph[0x01] = NULL;
		font->glyph[0x02] = NULL;
		font->glyph[0x03] = NULL;
		font->glyph[0x04] = NULL;
		font->glyph[0x05] = NULL;
		font->glyph[0x06] = NULL;
		font->glyph[0x07] = NULL;
		font->glyph[0x08] = NULL;
		font->glyph[0x09] = NULL;
		font->glyph[0x0A] = NULL;
		font->glyph[0x0B] = NULL;
		font->glyph[0x0C] = NULL;
		font->glyph[0x0D] = NULL;
		font->glyph[0x0E] = NULL;
		font->glyph[0x0F] = NULL;
		font->glyph[0x10] = NULL;
		font->glyph[0x11] = NULL;
		font->glyph[0x12] = NULL;
		font->glyph[0x13] = NULL;
		font->glyph[0x14] = NULL;
		font->glyph[0x15] = NULL;
		font->glyph[0x16] = NULL;
		font->glyph[0x17] = NULL;
		font->glyph[0x18] = NULL;
		font->glyph[0x19] = NULL;
		font->glyph[0x1A] = NULL;
		font->glyph[0x1B] = NULL;
		font->glyph[0x1C] = NULL;
		font->glyph[0x1D] = NULL;
		font->glyph[0x1E] = NULL;
		font->glyph[0x1F] = NULL;

		font->glyph[0x20] = make_glyph(6, 8, STR_SPC);
		font->glyph[0x21] = make_glyph(6, 8, STR_BANG);
		font->glyph[0x22] = make_glyph(6, 8, STR_DQUOTE);

		return font;
	}

	return NULL;
}
