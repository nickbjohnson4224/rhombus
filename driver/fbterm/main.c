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
	struct vfs_obj *file;
	size_t i;

	file = vfs_get(index);

	mutex_spin(&file->mutex);
	for (i = 0; i < size; i++) {
		fbterm_print(buffer[i]);
	}

	screen_flip();
	mutex_free(&file->mutex);

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
	struct vfs_obj *file;
	size_t i;

	file = vfs_get(index);

	mutex_spin(&file->mutex);
	for (i = 0; i < size; i++) {
		buffer[i] = fbterm_getch();

		if (buffer[i] == '\0') {
			break;
		}
	}
	mutex_free(&file->mutex);

	return i;
}

void fbterm_event(uint64_t source, uint64_t value) {
	int type = value >> 62;
	int data = value & ~(0x3LL << 62);

	if (type == 0x0) {
		keyboard_event(data);
	}
	if (type == 0x3) {
		fbterm_resize((data >> 16) & 0xffff, data & 0xffff);
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
	int pid;

	if (argc < 3) {
		wmanager = io_find("/sys/wmanager");
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
	root->type = FS_TYPE_FILE | FS_TYPE_CHAR;
	root->size = 0;
	root->acl = acl_set_default(root->acl, PERM_WRITE | PERM_READ);
	vfs_set(0, root);

	// set up screen
	fb = fb_cons(fb_dev);
	font = font_load("builtin");
	screen.font = font;
	fb_getmode(fb, &w, &h);
	screen_resize(w, h);
	screen_flip();
	event_register(fb_dev, fbterm_event);

	// set up keyboard
	event_register(kbd_dev, fbterm_event);

	rcall_set("clear", fbterm_rcall_clear);
	rcall_set("getfg", fbterm_rcall_getfg);
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
