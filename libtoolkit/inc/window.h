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

#ifndef _TOOLKIT_WINDOW_H
#define _TOOLKIT_WINDOW_H

struct widget;

typedef void (*handler_t)(struct widget *widget, const char *event);

enum window_flags {
	LISTEN_EVENTS = 0x1,
	CONSTANT_SIZE = 0x2,
	FLOATING	  = 0x4
};

struct window {
	struct fb *fb;
	struct widget *widget;
	handler_t handler;
};

struct window *create_window_from_widget(const char *widget);
struct window *create_window_from_file(const char *filename);
struct window *create_window_from_store(const char *window);
void destroy_window(struct window *window);
void draw_window  (struct window *window);
void update_window(struct window *window);
void resize_window(struct window *window, int width, int height);
void get_window_size(struct window *window, int *width, int *height);
struct widget *find_widget(struct window *window, const char *name);
void window_register(struct window *window, handler_t handler);
void add_window_flags(struct window *window, enum window_flags flags);
void clear_window_flags(struct window *window, enum window_flags flags);

#endif
