/*
 * Copyright (C) 2011 Jaagup Repan
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

#include "wmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <exec.h>
#include <proc.h>
#include <page.h>

uint64_t vgafd;
uint8_t *screen;
size_t screen_width, screen_height;

char *wmanager_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	if (strncmp(args, "setmode ", 8) == 0) {
		size_t width, height;
		if (sscanf(args + 8, "%i %i", &width, &height) != 2) {
			return strdup("");
		}
		if (set_window_size(file->index, RP_PID(source), width, height) != 0) {
			return strdup("");
		}
		return strdup("T");
	}

	if (strcmp(args, "register") == 0) {
		return strdup("T");
	}

	return NULL;
}

int wmanager_share(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off) {
	if (off != 0) {
		return -1;
	}
	return set_window_bitmap(file->index, RP_PID(source), buffer, size);
}

int wmanager_sync(uint64_t source, struct vfs_obj *file) {
	memset(screen, 0, screen_width * screen_height * 4);
	struct window_t *window;
	for (window = windows; window; window = window->next) {
		draw_window(window);
	}
	draw_cursor();
	sync(vgafd);
	return 0;
}

struct vfs_obj *wmanager_cons(uint64_t source, int type) {
	static int next_index = 1;
	struct vfs_obj *fobj = NULL;

	switch (type) {
	case RP_TYPE_FILE:
		fobj        = calloc(sizeof(struct vfs_obj), 1);
		fobj->type  = type;
		fobj->size  = 0;
		fobj->link  = 0;
		fobj->data  = NULL;
		fobj->index = next_index++;
		fobj->acl   = acl_set_default(fobj->acl, PERM_READ | PERM_WRITE);
		break;
	}
	
	return fobj;
}

int wmanager_push(uint64_t source, struct vfs_obj *file) {
	return add_window(file->index, RP_PID(source));
}

int wmanager_pull(uint64_t source, struct vfs_obj *file) {
	return remove_window(file->index, RP_PID(source));
}

void wmanager_event(uint64_t source, uint64_t value) {
	int type = value >> 62;
	int data = value & ~(0x3LL << 62);

	if (type == 0x1) { 
		mouse_move((data >> 16) & 0xffff, data & 0xffff);
	}
	if (type == 0x2) {
		mouse_click(data & ~(0x3LL << 62));
	}
	if (type == 0x3) {
		mouse_release(data);
	}

	if (active_window) {
		event(RP_CONS(active_window->owner, 0), value);
	}
}

int main(int argc, char **argv) {
	struct vfs_obj *root;

	stdout = stderr = fopen("/dev/serial", "w");

	if (fork() < 0) {
		exec("/sbin/vga");
	}
	mwait(PORT_CHILD, 0);
	if (fork() < 0) {
		exec("/sbin/mouse");
	}
	mwait(PORT_CHILD, 0);

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = RP_TYPE_DIR;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, PERM_READ | PERM_WRITE);
	vfs_set_index(0, root);

	di_wrap_rcall(wmanager_rcall);
	di_wrap_share(wmanager_share);
	di_wrap_sync (wmanager_sync);
	vfs_wrap_cons(wmanager_cons);
	vfs_wrap_push(wmanager_push);
	vfs_wrap_pull(wmanager_pull);
	vfs_wrap_init();

	io_link("/sys/wmanager", RP_CONS(getpid(), 0));

	vgafd = io_find("/dev/vga0");
	sscanf(rcall(vgafd, "getmode"), "%i %i", &screen_width, &screen_height);
	screen = malloc(screen_width * screen_height * 4);
	memset(screen, 0, screen_width * screen_height * 4);
	share(vgafd, screen, screen_width * screen_height * 4, 0, PROT_READ);

	event_register(io_find("/dev/mouse"), wmanager_event);

	if (fork() < 0) {
		exec("/bin/testapp");
	}

	_done();

	return 0;
}
