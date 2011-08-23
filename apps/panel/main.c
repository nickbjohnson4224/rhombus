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

#include <exec.h>
#include <proc.h>
#include <natio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toolkit/toolkit.h>

void panel_window_event(struct widget *widget, const char *event) {
	if (!strcmp(get_name(widget), "launch_terminal") && !strcmp(event, "clicked")) {
		if (fork() < 0) {
			exec("/sbin/fbterm");
			exit(1);
		}
	}
	if (!strcmp(get_name(widget), "launch_testtoolkit") && !strcmp(event, "clicked")) {
		if (fork() < 0) {
			exec("/bin/testtoolkit");
			exit(1);
		}
	}
	if (!strcmp(get_name(widget), "halt") && !strcmp(event, "clicked")) {
		if (fork() < 0) {
			exec("/bin/halt");
			exit(1);
		}
	}
}

int main(int argc, char **argv) {
	struct window *window;
	char *ret;

	if (init_toolkit()) {
		fprintf(stderr, "%s: initializing toolkit failed\n", argv[0]);
		return 1;
	}

	window = create_window_from_file("/etc/panelui.txt");
	if (!window) {
		fprintf(stderr, "%s: creating window failed\n", argv[0]);
		return 1;
	}

	ret = rcall(get_resource_pointer(window), "setpanel");
#if 0
	//fixme
	fprintf(stderr, "'%s'\n", ret);
	if (!ret || strcmp(ret, "T")) {
		fprintf(stderr, "%s: setting panel failed\n", argv[0]);
		return 1;
	}
#endif
	free(ret);

	window_register(window, panel_window_event);

	_done();
	return 0;
}
