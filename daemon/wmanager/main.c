/*
 * Copyright (C) 2011 Jaagup Repan
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

#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <mutex.h>
#include <stdio.h>
#include <exec.h>
#include <proc.h>
#include <page.h>
#include <vfs.h>

#include "wmanager.h"

uint64_t vgafd, mousefd, kbdfd;
bool winkey;
int next_index = 1;

char *wmanager_rcall_listmodes(uint64_t source, uint32_t index, int argc, char **argv) {
	return strdup("any");
}

char *wmanager_rcall_createwindow(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;
	char buffer[32];

	sprintf(buffer, "/sys/wmanager/%i", next_index);
	io_cons(buffer, TYPE_FILE | TYPE_GRAPH | TYPE_EVENT);
	window = find_window(next_index - 1, 0);
	if (!window) {
		fprintf(stderr, "wmanager: unable to create window\n");
		return NULL;
	}
	window->owner = RP_PID(source);
	sprintf(buffer, "%i", next_index - 1);

	return strdup(buffer);
}

char *wmanager_rcall_setmode(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;
	int width, height;
	int old_flags;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	if (argc != 4) return NULL;

	old_flags = window->flags;
	width  = atoi(argv[1]);
	height = atoi(argv[2]);

	resize_window(window, width, height, false);
	if (!(window->flags & CONSTANT_SIZE) && !(window->flags & FLOATING)) {
		// window must be CONSTNAT_SIZE or FLOATING for resizing to make sense
		window->flags |= FLOATING;
	}

	if (!(old_flags & FLOATING)) {
		update_tiling();
	}

	return strdup("T");
}

char *wmanager_rcall_getmode(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	return saprintf("%i %i 32", window->width, window->height);
}

char *wmanager_rcall_syncrect(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;
	int x, y, w, h;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	if (argc != 5) return NULL;

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	w = atoi(argv[3]);
	h = atoi(argv[4]);

	update_screen(window->x + x, window->y + y, window->x + x + w, window->y + y + h);

	return strdup("T");
}

char *wmanager_rcall_unshare(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	if (!window->bitmap) return NULL;

	mutex_spin(&window->mutex);
	page_free(window->bitmap, window->width * window->height * 4);
	window->bitmap = NULL;
	mutex_free(&window->mutex);

	return strdup("T");
}

char *wmanager_rcall_register(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	window->flags |= LISTEN_EVENTS;

	return strdup("T");
}

char *wmanager_rcall_deregister(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	window->flags &= ~LISTEN_EVENTS;

	return strdup("T");
}

char *wmanager_rcall_getwindowflags(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	return saprintf("%i", window->flags);
}

char *wmanager_rcall_setwindowflags(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	if (argc != 2) return NULL;

	window->flags = atoi(argv[1]);

	return strdup("T");
}

int wmanager_share(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off) {
	struct window_t *window = find_window(index, RP_PID(source));

	if (off != 0) {
		return -1;
	}
	if (!window) {
		return -1;
	}
	if (size != window->width * window->height * 4) {
		return -1;
	}

	mutex_spin(&window->mutex);
	if (window->bitmap) {
		page_free(window->bitmap, window->width * window->height * 4);
	}
	window->bitmap = buffer;
	mutex_free(&window->mutex);
	return 0;
}

int wmanager_sync(uint64_t source, uint32_t index) {
	struct window_t *window = find_window(index, RP_PID(source));
	if (!window) {
		return -1;
	}
	update_screen(window->x - 1, window->y - 1, window->x + window->width + 1, window->y + window->height + 1);
	return 0;
}

struct vfs_obj *wmanager_cons(uint64_t source, int type) {
	struct vfs_obj *fobj = NULL;

	if (RP_PID(source) != getpid()) {
		return NULL;
	}

	if (type & TYPE_FILE) {
		fobj        = calloc(sizeof(struct vfs_obj), 1);
		fobj->type  = type;
		fobj->size  = 0;
		fobj->link  = 0;
		fobj->data  = NULL;
		fobj->index = next_index++;
		fobj->acl   = acl_set_default(fobj->acl, PERM_READ | PERM_WRITE);
	}
	
	return fobj;
}

int wmanager_push(uint64_t source, struct vfs_obj *file) {
	if (RP_PID(source) != getpid()) {
		return -1;
	}

	return add_window(file->index);
}

int wmanager_pull(uint64_t source, struct vfs_obj *file) {
	return remove_window(file->index, RP_PID(source));
}

void wmanager_event(uint64_t source, uint64_t value) {
	int type = value >> 62;
	int data = value & ~(0x3LL << 62);
	bool released;

	if (type == 0x0 && source == kbdfd) { // keyboard
		released = data & 0x00400000 ? true : false;
		data &= ~0x00400000;
		if (data == 0x00800004) {
			winkey = !released;
		}
	}
	if (type == 0x1 && source == mousefd) { 
		mouse_move((data >> 16) & 0xffff, data & 0xffff);
	}
	if (type == 0x2 && source == mousefd) {
		mouse_buttons(data & ~(0x3LL << 62));
	}
	if (type == 0x3 && source == vgafd) {
		resize_screen((data >> 16) & 0xffff, data & 0xffff);
		return; // don't forward
	}

	if (active_window && (active_window->flags & LISTEN_EVENTS)) {
		event(RP_CONS(active_window->owner, 0), value);
	}
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	size_t width, height;

	stdout = stderr = fopen("/dev/serial", "w");

	if (fork() < 0) {
		exec("/sbin/mouse");
	}
	mwait(PORT_CHILD, 0);

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = TYPE_DIR;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, PERM_READ | PERM_WRITE);
	vfs_set(0, root);

	rcall_set("listmodes",      wmanager_rcall_listmodes);
	rcall_set("createwindow",   wmanager_rcall_createwindow);
	rcall_set("setmode",        wmanager_rcall_setmode);
	rcall_set("getmode",        wmanager_rcall_getmode);
	rcall_set("unshare",        wmanager_rcall_unshare);
	rcall_set("register",       wmanager_rcall_register);
	rcall_set("deregister",     wmanager_rcall_deregister);
	rcall_set("getwindowflags", wmanager_rcall_getwindowflags);
	rcall_set("setwindowflags", wmanager_rcall_setwindowflags);
	rcall_set("syncrect",       wmanager_rcall_syncrect);

	di_wrap_share(wmanager_share);
	di_wrap_sync (wmanager_sync);
	vfs_set_cons(wmanager_cons);
	vfs_set_push(wmanager_push);
	vfs_set_pull(wmanager_pull);
	vfs_init();

	io_link("/sys/wmanager", RP_CONS(getpid(), 0));

	vgafd = io_find("/dev/svga0");
	sscanf(rcall(vgafd, "getmode"), "%i %i", &width, &height);
	resize_screen(width, height);

	mousefd = io_find("/dev/mouse");
	kbdfd = io_find("/dev/kbd");
	event_register(mousefd, wmanager_event);
	event_register(kbdfd, wmanager_event);
	event_register(vgafd, wmanager_event);

	if (fork() < 0) {
		exec("/sbin/fbterm");
	}

	_done();

	return 0;
}
