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
#include <ctype.h>
#include <proc.h>
#include <page.h>

#include "tty.h"

size_t tty_write(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		tty_print(buffer[i]);
	}

	tty_flip();

	return size;
}

size_t tty_read(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;

	for (i = 0; i < size; i++) {
		buffer[i] = tty_getch();

		if (buffer[i] == '\0') {
			break;
		}
	}

	return i;
}

void tty_event(uint64_t source, uint64_t value) {
	char c;
	static bool ctrl = false;

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

		tty_print('^');
		tty_print('C');

		tty_print('\n');
		tty_buffer('\n');
		return;
	}

	tty_buffer(c);

	if (isprint(c) && tty->mode & MODE_ECHO) {
		tty_print(c);
		tty_flip();
	}
}

char *tty_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	char buffer[10];
	
	if (!strcmp(args, "getfg")) {
		sprintf(buffer, "%d", getpid());
		return strdup(buffer);
	}
	
	if (!strcmp(args, "clear")) {
		tty_clear();
		tty->cursor = 0x0000;
	}
	
	return NULL;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	uint64_t kbd;
	
	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_FILE;
	root->size = 0;
	root->acl = acl_set_default(root->acl, PERM_WRITE | PERM_READ);
	vfs_set_index(0, root);

	tty_init();

	kbd = io_find("/dev/kbd");
	event_register(kbd, tty_event);

	di_wrap_read (tty_read);
	di_wrap_write(tty_write);
	di_wrap_rcall(tty_rcall);
	vfs_wrap_init();

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
