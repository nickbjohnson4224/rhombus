/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <stdio.h>
#include <proc.h>

#include "keyboard.h"

size_t kbd_read(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = pop_char();

		if (buffer[i] == 'D') {
			break;
		}
	}

	return i;
}

void kbd_irq(struct msg *msg) {
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

int main(int argc, char **argv) {
	struct vfs_obj *root;

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = FOBJ_FILE;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, FS_PERM_READ);
	vfs_set_index(0, root);

	/* set up interface */
	di_wrap_read(kbd_read);
	di_wrap_irq (1, kbd_irq);
	vfs_wrap_init();

	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
