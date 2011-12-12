/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include "fbterm.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <rho/proc.h>
#include <rho/exec.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/io.h>

FT_Face face;

size_t fbterm_write(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		fbterm_print(buffer[i]);
	}

	screen_flip();

	return size;
}

size_t fbterm_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = fbterm_getch();

		if (buffer[i] == '\0') {
			break;
		}
	}

	return i;
}

char *fbterm_rcall_set_fgjob(struct robject *self, rp_t source, int argc, char **argv) {
	extern uint32_t fgjob_pid;
	
	if (argc != 2) {
		return NULL;
	}

	fgjob_pid = atoi(argv[1]);

	return strdup("T");
}

char *fbterm_rcall_clear(struct robject *self, rp_t source, int argc, char **argv) {

	fbterm_clear();

	return strdup("T");
}

char *fbterm_rcall_set_fgcolor(struct robject *self, rp_t source, int argc, char **argv) {
	
	if (argc != 2) {
		return NULL;
	}

	screen.fg = atoi(argv[1]);
	screen_sync();

	return strdup("T");
}

char *fbterm_rcall_set_bgcolor(struct robject *self, rp_t source, int argc, char **argv) {
	
	if (argc != 2) {
		return NULL;
	}

	screen.bg = atoi(argv[1]);
	screen_sync();

	return strdup("T");
}

void fbterm_key_event(rp_t source, int argc, char **argv) {
	
	if (argc != 3) return;

	if (!strcmp(argv[1], "press")) {
		keyboard_event(atoi(argv[2]), true);
	}
	else if (!strcmp(argv[1], "release")) {
		keyboard_event(atoi(argv[2]), false);
	}
}

void fbterm_graph_event(rp_t source, int argc, char **argv) {
	int w, h;
	
	if (argc != 4) return;

	if (!strcmp(argv[1], "resize")) {
		w = atoi(argv[2]);
		h = atoi(argv[3]);

		fbterm_resize(w, h);
		screen_flip();
	}
}

int main(int argc, char **argv) {
	struct robject *term;
	FT_Library library;
	uint64_t kbd_dev;
	uint64_t fb_dev;
	int w, h;
	int pid;

	rdi_init();

	term = rdi_file_cons(1, ACCS_READ | ACCS_WRITE);

	robject_set_data(term, "type", (void*) "term");

	// get font size
	if (FT_Init_FreeType(&library)) {
		fprintf(stderr, "%s: initing FreeType failed\n", argv[0]);
		return 1;
	}
	if (FT_New_Face(library, "/etc/dejavu.ttf", 0, &face)) {
		fprintf(stderr, "%s: loading font /etc/dejavu.ttf failed\n", argv[0]);
		return 1;
	}
	if (FT_Set_Pixel_Sizes(face, 0, screen.font_size)) {
		fprintf(stderr, "%s: setting font size to %i failed\n", argv[0], screen.font_size);
		return 1;
	}
	screen.cell_width  = ceil(face->max_advance_width  / (double) face->units_per_EM * screen.font_size) + 1;
	screen.cell_height = ceil(face->max_advance_height / (double) face->units_per_EM * screen.font_size) + 1;

	// set up screen
	if (argc >= 3) {
		kbd_dev = fs_find(argv[1]);
		fb_dev  = fs_find(argv[2]);

		if (!kbd_dev) {
			fprintf(stderr, "%s: %s: keyboard not found\n", argv[0], argv[1]);
			return 1;
		}

		if (!fb_dev) {
			fprintf(stderr, "%s: %s: graphics device not found\n", argv[0], argv[2]);
			return 1;
		}
		
		fb = fb_cons(fb_dev);
	}
	else {
		fb = fb_createwindow();
		fb_dev  = fb->rp;
		kbd_dev = fb->rp;
	}
	if (!fb) {
		fprintf(stderr, "%s: setting up framebuffer failed\n", argv[0]);
		return 1;
	}

	fb_getmode(fb, &w, &h);
	screen_resize(w, h);
	screen_flip();

	// listen to graphics events
	event_subscribe(fb_dev);
	event_hook("graph", fbterm_graph_event);

	// set up keyboard
	event_subscribe(kbd_dev);
	event_hook("key", fbterm_key_event);

	robject_set_call(term, "clear",       fbterm_rcall_clear,       STAT_WRITER);
	robject_set_call(term, "set_fgjob",   fbterm_rcall_set_fgjob,   STAT_WRITER);
	robject_set_call(term, "set_fgcolor", fbterm_rcall_set_fgcolor, STAT_WRITER);
	robject_set_call(term, "set_bgcolor", fbterm_rcall_set_bgcolor, STAT_WRITER);
	rdi_global_read_hook = fbterm_read;
	rdi_global_write_hook = fbterm_write;

	// launch shell
	pid = fork();
	if (pid == 0) {
		setenv("PATH", "/bin");
		stdout = stderr = fdopen(RP_CONS(getppid(), 0), "w");
		stdin = fdopen(RP_CONS(getppid(), 0), "r");
		exec("/bin/fish");
	}

	// daemonize
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
