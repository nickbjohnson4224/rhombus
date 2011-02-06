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

#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stdbool.h>

struct tty_line {
	struct tty_line *next;
	struct tty_line *prev;
	char *data;
};

#define SCREEN_PADDR	0xB8000
#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25

#define MODE_COOK		0x0001 // Buffer input
#define MODE_ECHO		0x0002 // Print keyboard input
#define MODE_SIGNAL		0x0004 // Send signals

struct tty {
	int mode;
	bool mutex;

	/* screen state */
	uint16_t width;
	uint16_t height;
	uint16_t cursor;
	uint16_t c_base;
	uint16_t *screen;

	/* keyboard state */
	struct tty_line *line_buf_in;
	struct tty_line *line_buf_out;
	char *line;
	size_t line_pos;
	char *buffer;
	size_t buffer_top;
	size_t buffer_size;

} ttyv[8], *tty;

void tty_init  (void);
void tty_print (char c);
void tty_flip  (void);
void tty_switch(int n);

void tty_buffer(char c);
char tty_getch (void);

#endif/*TTY_H*/
