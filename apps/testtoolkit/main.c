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

#include <rho/proc.h>
#include <stdio.h>
#include <toolkit/toolkit.h>

void handler(struct widget *widget, const char *event) {
	printf("%s %s\n", get_name(widget), event);
}

int main(int argc, char **argv) {
	struct window *window;
	struct widget *widget;

	if (init_toolkit()) {
		fprintf(stderr, "%s: initializing toolkit failed\n", argv[0]);
		return 1;
	}

	window = create_window_from_store("testwindow");
	if (!window) {
		fprintf(stderr, "%s: creating window failed\n", argv[0]);
		return 1;
	}
	window_register(window, handler);

	widget = find_widget(window, "c");
	if (widget) {
		set_attribute_string(widget, "text", "CCC");
		update_window(window);
	}

	_done();
	return 0;
}
