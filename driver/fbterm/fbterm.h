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

#ifndef FBTERM_H
#define FBTERM_H

#include <stdint.h>
#include <graph.h>

/* graphics device **********************************************************/

extern struct fb *fb;

/* font rendering ***********************************************************/

struct glyph {
	int w, h; 		 // width/height of glyph
	uint8_t value[]; // contents of glyph (alpha channel)
};

struct font {
	int w, h;					// width/height of glyphs (maximum)
	uint32_t count;				// size of glyph vector
	struct glyph *def_glyph;	// default glyph
	struct glyph *glyph[];		// glyphs in order
};

struct cell {
	uint32_t fg; // foreground color
	uint32_t bg; // background color
	uint32_t ch; // code point of character
	uint32_t dirty;
};

int draw_cell(struct font *font, struct cell *c, int x, int y);

struct font *font_load(const char *path);

/* screen contents **********************************************************/

extern struct screen {
	struct font *font;
	struct cell *cell;
	uint32_t fg;
	uint32_t bg;
	int w;
	int h;
} screen;

int screen_resize(uint32_t x, uint32_t y);
int screen_print (int x, int y, uint32_t c);
int screen_scroll(void);
int screen_clear (void);
int screen_flip  (void);

/* terminal emulation *******************************************************/

int fbterm_print(uint32_t c);
int fbterm_reset(void);

/* line buffering ***********************************************************/

char fbterm_getch (void);
int  fbterm_buffer(int c);

#define MODE_ECHO
#define MODE_COOK

int  fbterm_setmode(int mode);

#endif/*FBTERM_H*/
