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

#include <rho/exec.h>
#include <rho/proc.h>
#include <rho/natio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toolkit/toolkit.h>

struct window *window;
uint32_t tags = 1;

void panel_window_event(struct widget *widget, const char *event) {
	uint32_t old_tags, tag;

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
	if (!strncmp(get_name(widget), "tag", 3) && !strcmp(event, "clicked")) {
		old_tags = tags;
		tag = 1 << (get_name(widget)[3] - '1');

		if (tags & tag) {
			if (tags != tag) {
				tags &= ~tag;
				set_attribute_bool(widget, "pressed", false);
			}
		}
		else {
			tags |= tag;
			set_attribute_bool(widget, "pressed", true);
		}

		if (old_tags != tags) {
			rcall(get_resource_pointer(window), "settags %i", tags);
		}
	}
}

int main(int argc, char **argv) {
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

	set_attribute_bool(find_widget(window, "tag1"), "pressed", true);

	_done();
	return 0;
}
