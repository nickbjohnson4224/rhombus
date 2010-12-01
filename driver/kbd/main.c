/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <stdio.h>
#include <proc.h>

#include "keyboard.h"

void kbd_init(int argc, char **argv) {
	struct fs_obj *root;

	root        = calloc(sizeof(struct fs_obj), 1);
	root->type  = FOBJ_FILE;
	root->size  = 0;
	root->inode = 0;
	root->acl   = acl_set_default(root->acl, ACL_READ);
	lfs_root(root);

	rirq(1);
}

size_t kbd_read(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = pop_char();

		if (buffer[i] == 'D') {
			break;
		}
	}

	return i;
}

void kbd_irq(void) {
	static bool shift = false;
	uint8_t scan;
	char c;

	scan = inb(0x60);

	if (scan & 0x80) {
		if (keymap[scan & 0x7F] == '\0') {
			shift = false;
		}
	}
	else if (keymap[scan & 0x7f] == '\0') {
		shift = true;
	}
	else {
		c = (shift) ? keymap[scan + 58] : keymap[scan];
		fwrite(&c, sizeof(char), 1, stdout);
		push_char(c);
	}
}

struct driver kbd_driver = {
	kbd_init,

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	kbd_read,
	NULL,
	NULL,
	NULL,

	kbd_irq,
};

int main(int argc, char **argv) {

	driver_init(&kbd_driver, argc, argv);

	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
