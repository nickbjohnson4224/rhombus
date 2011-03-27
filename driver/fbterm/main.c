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


size_t fbterm_write(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;
	size_t pos;

	pos = offset;
	for (i = 0; i < size; i++, pos++) {
		screen_print(buffer[i], pos % screen.w, pos / screen.w);
	}

	screen_flip();

	return size;
}

char *fbterm_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	char buffer[10];

	if (!strcmp(args, "getfg")) {
		sprintf(buffer, "%d", getpid());
		return strdup(buffer);
	}

	if (!strcmp(args, "clear")) {
		screen_clear();
	}

	return NULL;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	struct font *font;
	uint64_t kbd_dev;
	uint64_t fb_dev;

	if (argc < 3) {
		fprintf(stderr, "%s: insufficient arguments\n", argv[0]);
		return 1;
	}

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

	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_FILE;
	root->size = 0;
	root->acl = acl_set_default(root->acl, PERM_WRITE | PERM_READ);

	fb = fb_cons(fb_dev);

	font = font_load("builtin");
	screen.font = font;

	screen_resize(106, 48);
	screen_print(0, 0, '!');
	screen_print(1, 0, '!');
	screen_print(0, 1, '!');
	screen_flip();

	return 0;
}
