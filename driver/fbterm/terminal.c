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

#include <mutex.h>
#include <graph.h>

static bool mutex;
static int cursor = 0;
static int c_base = 0;

int fbterm_print(uint32_t c) {
	uint32_t bg;

	mutex_spin(&mutex);

	switch (c) {
	case '\0':
		break;
	case '\t':
		screen_print(cursor % screen.w, cursor / screen.w, ' ');
		cursor = (cursor + 8) - ((cursor % screen.w) % 8);
		break;
	case '\n':
		screen_print(cursor % screen.w, cursor / screen.w, ' ');
		c_base = cursor = (cursor - cursor % screen.w) + screen.w;
		break;
	case '\r':
		cursor = cursor - cursor % screen.w;
		c_base = cursor;
		break;
	case '\b':
		screen_print(cursor % screen.w, cursor / screen.w, ' ');
		if (c_base < cursor) cursor--;
		break;
	default:
		screen_print(cursor % screen.w, cursor / screen.w, c);
		cursor++;
		break;
	}

	if (cursor > screen.w * screen.h - 1) {
		screen_scroll();
		cursor -= screen.w;
		c_base -= screen.w;
	}

	// draw cursor
	bg = screen.bg;
	screen.bg = COLOR_LGRAY;
	screen_print(cursor % screen.w, cursor / screen.w, ' ');
	screen.bg = bg;
	
	mutex_free(&mutex);

	return 0;
}

int fbterm_clear(void) {
	
	screen_clear();

	cursor = 0;
	c_base = 0;

	return 0;
}
