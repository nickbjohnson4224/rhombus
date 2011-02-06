/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <mutex.h>
#include <page.h>

#include "tty.h"

static bool mutex;
static uint16_t *real_screen;
static uint16_t *fake_screen;

void tty_init(void) {
	int i;

	for (i = 0; i < 8; i++) {
		ttyv[i].mode   = MODE_COOK | MODE_ECHO | MODE_SIGNAL;
		ttyv[i].width  = SCREEN_WIDTH;
		ttyv[i].height = SCREEN_HEIGHT;
		ttyv[i].cursor = 0;
		ttyv[i].c_base = 0;
		ttyv[i].screen = calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(uint16_t));

		ttyv[i].line_buf_in  = NULL;
		ttyv[i].line_buf_out = NULL;
		ttyv[i].line = NULL;
		ttyv[i].line_pos = 0;
		ttyv[i].buffer = 0;
		ttyv[i].buffer_top = 0;
		ttyv[i].buffer_size = 0;
	}

	fake_screen = calloc(SCREEN_WIDTH * SCREEN_HEIGHT, sizeof(uint16_t));
	real_screen = aalloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t), PAGESZ);
	page_phys(real_screen, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t), 
		PROT_READ | PROT_WRITE, SCREEN_PADDR);

	tty_switch(0);

	for (i = 0; i < tty->width * tty->height; i++) {
		tty->screen[i] = real_screen[i] = fake_screen[i] = 0x0F20;
	}
}

void tty_print(char c) {
	uint16_t i;

	mutex_spin(&mutex);

	switch (c) {
		case '\0': 
			break;
		case '\t': 
			tty->cursor = (tty->cursor + 8) - (tty->cursor % 8); 
			break;
		case '\n':
			tty->screen[tty->cursor] = 0x0000;
			tty->c_base = tty->cursor = (tty->cursor - tty->cursor % tty->width) + tty->width; 
			break;
		case '\r': 
			tty->cursor = (tty->cursor - tty->cursor % tty->width); 
			tty->c_base = tty->cursor;
			break;
		case '\b':
			if (tty->c_base < tty->cursor) tty->cursor--; 
			tty->screen[tty->cursor] = 0x0F00; 
			break;
		default: 
			tty->screen[tty->cursor++] = (uint16_t) (c | 0x0F00); 
			break;
	}

	if (tty->cursor >= tty->width * tty->height) {
		for (i = 0; i < tty->width * (tty->height - 1); i++) {
			tty->screen[i] = tty->screen[i + tty->width];
		}

		for (i = tty->width * (tty->height - 1); i < tty->width * tty->height; i++) {
			tty->screen[i] = 0x0F20;
		}
		
		tty->c_base -= tty->width;
		tty->cursor -= tty->width;
	}

	mutex_free(&mutex);
}

void tty_flip(void) {
	uint16_t i;

	mutex_spin(&tty->mutex);

	for (i = 0; i < tty->width * tty->height; i++) {
		if (tty->screen[i] != fake_screen[i]) {
			real_screen[i] = tty->screen[i];
			fake_screen[i] = tty->screen[i];
		}
	}

	outb(0x3D4, 14);
	outb(0x3D5, tty->cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, tty->cursor & 0xFF);

	mutex_free(&tty->mutex);
}

void tty_switch(int n) {

	if (n < 0 || n > 7) {
		return;
	}

	tty = &ttyv[n];
}
