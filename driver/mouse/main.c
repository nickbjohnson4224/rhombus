/*
 * Copyright (C) 2011 Jaagup Repän <jrepan@gmail.com>
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail dot com>
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

#include <driver.h>
#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

struct event_list *event_list;

void wait_signal() {
	while (inb(0x64) & 0x2);
}

void command(uint8_t byte) {
	wait_signal();
	outb(0x64, 0xd4);
	wait_signal();
	outb(0x60, byte);
	while ((inb(0x64) & 0x21) == 0x21 && inb(0x60) != 0xfa); // ACK
}

char *mouse_register(uint64_t source, uint32_t index, int argc, char **argv) {
	struct vfs_obj *file;

	file = vfs_get(index);

	mutex_spin(&file->mutex);
	event_list = event_list_add(event_list, source);
	mutex_free(&file->mutex);

	return strdup("T");
}

char *mouse_deregister(uint64_t source, uint32_t index, int argc, char **argv) {
	struct vfs_obj *file;

	file = vfs_get(index);

	mutex_spin(&file->mutex);
	event_list = event_list_del(event_list, source);
	mutex_free(&file->mutex);

	return strdup("T");
}


int main(int argc, char **argv) {
	struct vfs_obj *root;
	uint8_t bytes[3];
	size_t curbyte = 0;
	bool first;
	int buttons = 0, prevbuttons = 0;
	int16_t dx = 0, dy = 0;

	command(0xa8);  // enable aux. PS2
	command(0xf6);  // load default config
	command(0xf4);  // enable mouse
	command(0xf3);  // set sample rate:
	outb(0x60, 10); // 10 samples per second

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = RP_TYPE_EVENT;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, PERM_READ | PERM_WRITE);
	vfs_set(0, root);

	rcall_set("register",   mouse_register);
	rcall_set("deregister", mouse_deregister);
	vfs_init();

	io_link("/dev/mouse", RP_CONS(getpid(), 0));
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);

	while (1) {
		first = true;
		while ((inb(0x64) & 0x21) != 0x21) {
			if (first) {
				if (dx || dy) {
					eventl(event_list, 0x1LL << 62 | (dx << 16) & 0xffff0000 | dy & 0xffff);
					dx = dy = 0;
				}
				if (buttons != prevbuttons) {
					eventl(event_list, 0x2LL << 62 | buttons);
					prevbuttons = buttons;
				}
			}
			first = false;
			sleep();
		}

		bytes[curbyte++] = inb(0x60);

		if (curbyte == 1) {
			buttons = bytes[0] & 0x7;
		}
		if (curbyte == 3) {
			curbyte = 0;
			dx += bytes[1] - ((bytes[0] & 0x10) ? 256 : 0);
			dy -= bytes[2] - ((bytes[0] & 0x20) ? 256 : 0);
		}
	}
	
	return 0;
}
