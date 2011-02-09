/*
 * Copyright (C) 2009, 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <string.h>
#include <stdlib.h>
#include <driver.h>
#include <mutex.h>
#include <proc.h>

#include "tty.h"

void _push_line(char *l) {
	struct tty_line *line;
	
	line = calloc(sizeof(struct tty_line), 0);
	line->data = l;

	mutex_spin(&tty->mutex);
	if (!tty->line_buf_in) {
		tty->line_buf_in  = line;
		tty->line_buf_out = line;
		line->next = NULL;
		line->prev = NULL;
	}
	else {
		line->next = NULL;
		line->prev = tty->line_buf_out;
		tty->line_buf_out->next = line;
		tty->line_buf_out = line;
	}
	mutex_free(&tty->mutex);
}

char *_pop_line(void) {
	struct tty_line *line;
	char *data;

	mutex_spin(&tty->mutex);
	if (!tty->line_buf_out) {
		while (1) {
			mutex_free(&tty->mutex);
			sleep();
			mutex_spin(&tty->mutex);
			if (tty->line_buf_out) break;
		}
	}

	line = tty->line_buf_out;
	tty->line_buf_out = tty->line_buf_out->prev;
	if (!tty->line_buf_out) tty->line_buf_in = NULL;
	mutex_free(&tty->mutex);

	data = line->data;
	free(line);

	return data;
}

void tty_buffer(char c) {

	if (!tty->buffer) {
		tty->buffer = malloc(256);
		
		if (!tty->buffer) {
			abort();
		}

		tty->buffer_size = 256;
		tty->buffer_top = 0;
	}

	if (c == '\b') {
		if (tty->buffer_top) tty->buffer_top--;
	}
	else {
		tty->buffer[tty->buffer_top++] = c;
		if (tty->buffer_top >= tty->buffer_size) {
			tty->buffer_size += 256;
			tty->buffer = realloc(tty->buffer, tty->buffer_size);
		}
	}

	if (c == '\n' || tty->buffer_top > 195) {
		tty->buffer[tty->buffer_top++] = '\0';
		_push_line(tty->buffer);
		tty->buffer = NULL;
	}
}

char tty_getch() {
	char c;

	if (!tty->line) {
		tty->line = _pop_line();
		tty->line_pos = 0;
	}

	c = tty->line[tty->line_pos];
	tty->line_pos++;

	if (c == '\n') {
		free(tty->line);
		tty->line = NULL;
	}

	return c;
}
