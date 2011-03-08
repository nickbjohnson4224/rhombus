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

/* font rendering ***********************************************************/

struct fbterm_font {
	int width, height;
	uint32_t **bitmap;
};

extern struct fbterm_font *fbterm_font;

int fbterm_set_font(struct fbterm_font *font);

/* drawing functions ********************************************************/

struct fbterm_char {
	uint32_t fbcolor;
	uint32_t bgcolor;
	uint32_t c;
};

extern struct fbterm_screen {
	struct fbterm_char **charv;
	uint32_t fbcolor;
	uint32_t bgcolor;
} fbterm_screen;

int fbterm_setfg(uint32_t color);
int fbterm_setbg(uint32_t color);
int fbterm_print(int x, int y, uint32_t c);
int fbterm_clear(void);
int fbterm_flip (void);

/* terminal emulation *******************************************************/

int fbterm_write(char c);
int fbterm_reset(void);

/* line buffering ***********************************************************/

char fbterm_read   (void);
int  fbterm_buffer (int c);

#define MODE_ECHO
#define MODE_COOK

int  fbterm_setmode(int mode);

#endif/*FBTERM_H*/
