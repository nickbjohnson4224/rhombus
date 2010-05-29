/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/proc.h>
#include <stdlib.h>

#include "keyboard.h"

static struct line {
	struct line *next;
	struct line *prev;
	char *data;
} *line_buf_in, *line_buf_out;

uint32_t m_line_buf;

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
	static uint32_t mutex;
	static char *buffer;
	static size_t buffer_top;

	mutex_spin(&mutex);

	if (!buffer) {
		buffer = malloc(200);
		buffer_top = 0;
	}

	if (c == '\b') {
		buffer_top--;
	}
	else {
		buffer[buffer_top++] = c;
	}

	if (c == '\n' || buffer_top > 195) {
		buffer[buffer_top++] = '\0';
		push_line(buffer);
		buffer = NULL;
	}

	mutex_free(&mutex);
}

char pop_char() {
	static uint32_t mutex;
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
