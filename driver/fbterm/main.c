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
#include <driver.h>
#include <stdio.h>
#include <proc.h>
#include <exec.h>
#include <vfs.h>

size_t fbterm_write(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		fbterm_print(buffer[i]);
	}

	screen_flip();

	return size;
}

char *fbterm_rcall_set_fgjob(uint64_t source, uint32_t index, int argc, char **argv) {
	extern uint32_t fgjob_pid;
	
	if (argc != 2) {
		return NULL;
	}

	fgjob_pid = atoi(argv[1]);

	return strdup("T");
}

char *fbterm_rcall_clear(uint64_t source, uint32_t index, int argc, char **argv) {
	fbterm_clear();
	return strdup("T");
}

char *fbterm_rcall_set_fgcolor(uint64_t source, uint32_t index, int argc, char **argv) {
	
	if (argc != 2) {
		return NULL;
	}

	screen.fg = atoi(argv[1]);
	screen_sync();

	return strdup("T");
}

char *fbterm_rcall_set_bgcolor(uint64_t source, uint32_t index, int argc, char **argv) {
	
	if (argc != 2) {
		return NULL;
	}

	screen.bg = atoi(argv[1]);
	screen_sync();

	return strdup("T");
}

size_t fbterm_read(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = fbterm_getch();

		if (buffer[i] == '\0') {
			break;
		}
	}

	return i;
}

void fbterm_key_event(uint64_t source, int argc, char **argv) {
	
	if (argc != 3) return;

	if (!strcmp(argv[1], "press")) {
		keyboard_event(atoi(argv[2]), true);
	}
	else if (!strcmp(argv[1], "release")) {
		keyboard_event(atoi(argv[2]), false);
	}
}

void fbterm_graph_event(uint64_t source, int argc, char **argv) {
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
	struct font *font;
	uint64_t kbd_dev;
	uint64_t fb_dev;
	uint64_t wmanager;
	char *ret;
	int w, h;
	int pid;

	if (argc < 3) {
		wmanager = fs_find("/sys/wmanager");
		if (!wmanager) {
			fprintf(stderr, "%s: couldn't find wmanager\n", argv[0]);
			return 1;
		}

		ret = rcall(wmanager, "createwindow");
		kbd_dev = fb_dev = ator(ret);
		free(ret);

		if (!fb_dev) {
			fprintf(stderr, "%s: creating window failed\n", argv[0]);
			return 1;
		}
	}
	else {
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
	}

	index_set(0, resource_cons(FS_TYPE_FILE | FS_TYPE_CHAR, PERM_READ | PERM_WRITE));

	// set up screen
	fb = fb_cons(fb_dev);
	font = font_load("builtin");
	screen.font = font;
	fb_getmode(fb, &w, &h);
	screen_resize(w, h);
	screen_flip();
	event_register(fb_dev);
	event_set("graph", fbterm_graph_event);

	// set up keyboard
	event_register(kbd_dev);
	event_set("key", fbterm_key_event);

	rcall_set("clear", fbterm_rcall_clear);
	rcall_set("set_fgjob", fbterm_rcall_set_fgjob);
	rcall_set("set_fgcolor", fbterm_rcall_set_fgcolor);
	rcall_set("set_bgcolor", fbterm_rcall_set_bgcolor);
	di_wrap_write(fbterm_write);
	di_wrap_read (fbterm_read);
	vfs_init();

	// launch shell
	pid = fork();
	if (pid < 0) {
		setenv("PATH", "/bin");
		stdout = stderr = fdopen(RP_CONS(-pid, 0), "w");
		stdin = fdopen(RP_CONS(-pid, 0), "r");
		exec("/bin/fish");
	}

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
