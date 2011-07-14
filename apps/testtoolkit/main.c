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

#include <graph.h>
#include <proc.h>
#include <stdio.h>
#include <toolkit/toolkit.h>

int main(int argc, char **argv) {
	struct window *window;

	init_toolkit();

	window = create_window("label");
	if (!window) {
		fprintf(stderr, "%s: creating window failed\n", argv[0]);
		return 1;
	}

	set_attribute_int(window->widget, "foreground", COLOR_WHITE); //fixme: default value
	set_attribute_int(window->widget, "background", COLOR_BLACK);
	set_attribute_string(window->widget, "text", "Hello, world!");
	draw_widget(window->widget);

	_done();
	return 0;
}
