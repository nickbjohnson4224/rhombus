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

#ifndef WIDGET_H
#define WIDGET_H

#include <graph.h>

struct widget {
	int ref;
	int x, y;
	int width, height;
};

void init_toolkit(struct fb *fb);
void close_toolkit();

struct widget *add_widget(const char *widget, int x, int y, int width, int height);
void free_widget(struct widget *widget);
int draw_widget(struct widget *widget);

void set_position(struct widget *widget, int x, int y);
int set_size(struct widget *widget, int width, int height);
int set_attribute_int(struct widget *widget, const char *name, int value);
int set_attribute_double(struct widget *widget, const char *name, double value);
int set_attribute_string(struct widget *widget, const char *name, const char *value);
int get_attribute_int(struct widget *widget, const char *name);
double get_attribute_double(struct widget *widget, const char *name);
const char *get_attribute_string(struct widget *widget, const char *name);

#endif
