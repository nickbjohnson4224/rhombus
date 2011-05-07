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
#include <exec.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <proc.h>

size_t fbterm_write(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		fbterm_print(buffer[i]);
	}

	screen_flip();

	return size;
}

char *fbterm_rcall_getfg(uint64_t source, uint32_t index, int argc, char **argv) {
	char buffer[10];

	sprintf(buffer, "%d", getpid());
	return strdup(buffer);
}

char *fbterm_rcall_clear(uint64_t source, uint32_t index, int argc, char **argv) {
	fbterm_clear();
	return strdup("T");
}

size_t fbterm_read(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = fbterm_getch();

		if (buffer[i] == '\0') {
			break;
		}
	}

	return i;
}

void fbterm_event(uint64_t source, uint64_t value) {
	char c;
	static bool ctrl = false;

	if (value >> 62) {
		// not keyboard event
		return;
	}

	if (value & 0x00400000) {
		if (value == 0x00C00001) {
			ctrl = false;
		}
		return;
	}

	if (value & 0x00800000) {
		if (value == 0x00800001) {
			ctrl = true;
		}
		return;
	}

	c = value;

	if (tolower(c) == 'c' && ctrl) {
		kill(-getpid(), SIGINT);

		fbterm_print('^');
		fbterm_print('C');

		fbterm_print('\n');
		fbterm_buffer('\n');
		return;
	}

	fbterm_buffer(c);

	if (isprint(c)) {
		fbterm_print(c);
		screen_flip();
	}
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	struct font *font;
	uint64_t kbd_dev;
	uint64_t fb_dev;
	uint64_t wmanager;
	char *ret;
	int w, h;
	int index;
	int pid;

	if (argc < 3) {
		wmanager = io_find("/sys/wmanager");
		if (!wmanager) {
			fprintf(stderr, "%s: couldn't find wmanager\n", argv[0]);
			return 1;
		}

		ret = rcall(wmanager, "createwindow");
		if (!ret || sscanf(ret, "%i", &index) != 1) {
			fprintf(stderr, "%s: creating window failed\n", argv[0]);
			return 1;
		}

		kbd_dev = fb_dev = RP_CONS(RP_PID(wmanager), index);
		rcall(fb_dev, "setmode 320 240 32");
		rcall(fb_dev, "setwindowflags 2"); // not resizeable
		sync(fb_dev);
	}
	else {
		kbd_dev = io_find(argv[1]);
		fb_dev  = io_find(argv[2]);

		if (!kbd_dev) {
			fprintf(stderr, "%s: %s: keyboard not found\n", argv[0], argv[1]);
			return 1;
		}

		if (!fb_dev) {
			fprintf(stderr, "%s: %s: graphics device not found\n", argv[0], argv[2]);
			return 1;
		}
	}

	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_FILE;
	root->size = 0;
	root->acl = acl_set_default(root->acl, PERM_WRITE | PERM_READ);
	vfs_set_index(0, root);

	// set up screen
	fb = fb_cons(fb_dev);
	font = font_load("builtin");
	screen.font = font;
	fb_getmode(fb, &w, &h);
	screen_resize(w / font->w, h / font->h);
	screen_flip();

	// set up keyboard
	event_register(kbd_dev, fbterm_event);

	rcall_set("clear", fbterm_rcall_clear);
	rcall_set("getfg", fbterm_rcall_getfg);
	di_wrap_write(fbterm_write);
	di_wrap_read (fbterm_read);
	vfs_wrap_init();

	// launch shell
	pid = fork();
	if (pid < 0) {
		stdin = stdout = stderr = fdopen(RP_CONS(-pid, 0), "w");
		exec("/bin/fish");
	}

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
