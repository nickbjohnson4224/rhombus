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

#include <string.h>
#include <stdlib.h>
#include <driver.h>
#include <signal.h>
#include <mutex.h>
#include <proc.h>
#include <page.h>

#include "tty.h"

size_t tty_write(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		tty_print(buffer[i]);
	}

	tty_flip();

	return size;
}

size_t tty_read(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = tty_getch();

		if (buffer[i] == '\0') {
			break;
		}
	}

	return i;
}

void tty_irq(struct msg *msg) {
	const char keymap[] ="\
\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 \
\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

	static bool shift = false;
	uint8_t scan;
	char c;

	scan = inb(0x60);

	if (scan & 0x80) {
		if (keymap[scan & 0x7F] == '\0') {
			shift = false;
		}
	}
	else if (keymap[scan & 0x7F] == '\0') {
		shift = true;
	}
	else {
		c = (shift) ? keymap[scan + 58] : keymap[scan];

		if (c == 'C') {
			kill(-getpid(), SIGINT);
			tty_buffer('\n');
		}
		else {
			tty_buffer(c);
		}

		if (tty->mode & MODE_ECHO) {
			tty_print(c);
			tty_flip();
		}
	}
}

char *tty_rcall(struct vfs_obj *file, const char *args) {
	char buffer[10];
	
	if (!strcmp(args, "getfg")) {
		sprintf(buffer, "%d", getpid());
		return strdup(buffer);
	}
	
	return NULL;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	
	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_FILE;
	root->size = 0;
	root->acl = acl_set_default(root->acl, PERM_WRITE | PERM_READ);
	vfs_set_index(0, root);

	tty_init();

	di_wrap_read (tty_read);
	di_wrap_irq  (1, tty_irq);
	di_wrap_write(tty_write);
	di_wrap_rcall(tty_rcall);
	vfs_wrap_init();

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
