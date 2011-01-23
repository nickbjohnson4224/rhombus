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

#include <stdint.h>
#include <stdlib.h>
#include <mutex.h>
#include <proc.h>

#include "keyboard.h"

static struct line {
	struct line *next;
	struct line *prev;
	char *data;
} *line_buf_in, *line_buf_out;

bool m_line_buf;

void push_line(char *l) {
	struct line *line;
	
	line = malloc(sizeof(struct line));
	line->data = l;

	mutex_spin(&m_line_buf);
	if (!line_buf_in) {
		line_buf_in  = line;
		line_buf_out = line;
		line->next = NULL;
		line->prev = NULL;
	}
	else {
		line->next = NULL;
		line->prev = line_buf_out;
		line_buf_out->next = line;
		line_buf_out = line;
	}
	mutex_free(&m_line_buf);
}

char *pop_line(void) {
	struct line *line;
	char *data;

	mutex_spin(&m_line_buf);
	if (!line_buf_out) {
		mutex_free(&m_line_buf);
		while (!line_buf_out) sleep();
		mutex_spin(&m_line_buf);
	}

	line = line_buf_out;
	line_buf_out = line_buf_out->prev;
	if (!line_buf_out) line_buf_in = NULL;
	mutex_free(&m_line_buf);

	data = line->data;
	free(line);

	return data;
}

void push_char(char c) {
	static bool mutex;
	static char *buffer;
	static size_t buffer_top;
	static size_t buffer_size;

	mutex_spin(&mutex);

	if (!buffer) {
		buffer = malloc(256);
		buffer_size = 256;
		buffer_top = 0;
	}

	if (c == '\b') {
		if (buffer_top) buffer_top--;
	}
	else {
		buffer[buffer_top++] = c;
		if (buffer_top >= buffer_size) {
			buffer_size += 256;
			buffer = realloc(buffer, buffer_size);
		}
	}

	if (c == '\n' || buffer_top > 195) {
		buffer[buffer_top++] = '\0';
		push_line(buffer);
		buffer = NULL;
	}

	mutex_free(&mutex);
}

char pop_char() {
	static bool mutex;
	static char *line;
	static size_t line_pos;

	char c;

	mutex_spin(&mutex);

	if (!line) {
		line = pop_line();
		line_pos = 0;
	}

	c = line[line_pos];
	line_pos++;

	if (c == '\n') {
		free(line);
		line = NULL;
	}

	mutex_free(&mutex);

	return c;
}
