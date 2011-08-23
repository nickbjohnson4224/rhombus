/*
 * Copyright (C) 2011 Jaagup Repan <jrepan at gmail.com>
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
#include <mutex.h>
#include <stdio.h>
#include <exec.h>
#include <proc.h>
#include <page.h>

#include <rdi/vfs.h>
#include <rdi/io.h>

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
	fs_cons(buffer, FS_TYPE_FILE | FS_TYPE_GRAPH | FS_TYPE_EVENT);
	window = find_window(next_index - 1, 0);
	if (!window) {
		fprintf(stderr, "wmanager: unable to create window\n");
		return NULL;
	}
	window->owner = RP_PID(source);

	return rtoa(RP_CONS(getpid(), next_index - 1));
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
		// window must be CONSTANT_SIZE or FLOATING for resizing to make sense
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
	int old_flags;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	if (argc != 2) return NULL;

	old_flags = window->flags;
	window->flags = atoi(argv[1]);

	if ((old_flags & FLOATING) != (window->flags & FLOATING)) {
		update_tiling();
	}

	return strdup("T");
}

char *wmanager_rcall_getmouse(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	return saprintf("%i %i", mousex - window->x, mousey - window->y);
}

char *wmanager_rcall_setpanel(uint64_t source, uint32_t index, int argc, char **argv) {
	struct window_t *window;

	if (panel) return NULL;

	window = find_window(index, RP_PID(source));
	if (!window) return NULL;

	panel = window;
	panel->flags |= FLOATING;
	panel->tags = 0xffffffff;
	resize_window(panel, screen_width, panel->height, true);
	update_tiling();

	return strdup("T");
}

char *wmanager_rcall_settags(uint64_t source, uint32_t index, int argc, char **argv) {
	uint32_t new_tags;

	if (!panel || RP_PID(source) != panel->owner) return NULL;
	if (argc != 2) return NULL;

	new_tags = atoi(argv[1]);
	if (new_tags == 0) return NULL;

	if (new_tags != current_tags) {
		current_tags = new_tags;
		update_tiling();
	}

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
	if (size != window->width * window->height * sizeof(uint32_t)) {
		return -1;
	}

	mutex_spin(&window->mutex);
	if (window->bitmap) {
		page_free(window->bitmap, window->width * window->height * 4);
	}
	window->bitmap = (uint32_t*) buffer;
	mutex_free(&window->mutex);
	return 0;
}

void wmanager_sync(uint64_t source, uint32_t index) {
	struct window_t *window = find_window(index, RP_PID(source));
	
	if (!window) {
		return;
	}

	update_screen(window->x - 1, window->y - 1, window->x + window->width + 1, window->y + window->height + 1);
}

struct resource *wmanager_cons(uint64_t source, int type) {
	struct resource *fobj = NULL;

	if (RP_PID(source) != getpid()) {
		return NULL;
	}

	if (FS_IS_FILE(type)) {
		fobj        = resource_cons(FS_TYPE_FILE | FS_TYPE_GRAPH | FS_TYPE_EVENT, PERM_READ | PERM_WRITE);
		fobj->size  = 0;
		fobj->data  = NULL;
		fobj->index = next_index++;
		add_window(fobj->index);
	}
	
	return fobj;
}

void wmanager_key_event(uint64_t source, int argc, char **argv) {
	char *event_str;
	bool pressed;
	int data;

	if (argc != 3) return;
	if (source != kbdfd) return;

	if (!strcmp(argv[1], "press")) pressed = true;
	else if (!strcmp(argv[1], "release")) pressed = false;
	else return;

	data = atoi(argv[2]);

	if (data == WINKEY) {
		winkey = pressed;
	}

	if (winkey && pressed && (data == CHANGE_MAIN_WINDOW_KEY)) {
		if (active_window && (active_window != windows)) {
			bring_to_front(active_window);
			update_tiling();
		}
	}
	else if (active_window && (active_window->flags & LISTEN_EVENTS)) {
		event_str = saprintf("key %s %s", argv[1], argv[2]);
		event(RP_CONS(active_window->owner, 0), event_str);
		free(event_str);
	}
}

void wmanager_mouse_event(uint64_t source, int argc, char **argv) {
	char *event_str;
	
	if (source != mousefd) return;
	if (argc < 2) return;

	if (!strcmp(argv[1], "delta")) {
		if (argc != 4) return;

		mouse_move(atoi(argv[2]), atoi(argv[3]));
		
		if (active_window && (active_window->flags & LISTEN_EVENTS)) {
			event_str = saprintf("mouse delta %s %s", argv[2], argv[3]);
			event(RP_CONS(active_window->owner, 0), event_str);
			free(event_str);
		}
	}
	else if (!strcmp(argv[1], "button")) {
		if (argc != 3) return;

		mouse_buttons(atoi(argv[2]));

		if (active_window && (active_window->flags & LISTEN_EVENTS)) {
			event_str = saprintf("mouse button %s", argv[2]);
			event(RP_CONS(active_window->owner, 0), event_str);
			free(event_str);
		}
	}
}

void wmanager_graph_event(uint64_t source, int argc, char **argv) {
	
	if (source != vgafd) return;
	if (argc != 4) return;
	
	if (!strcmp(argv[1], "resize")) {
		resize_screen(atoi(argv[2]), atoi(argv[3]));
	}
}

int main(int argc, char **argv) {
	int width, height;

	stdout = stderr = fopen("/dev/serial", "w");

	if (fork() < 0) {
		exec("/sbin/mouse");
	}
	mwait(PORT_CHILD, 0);

	index_set(0, resource_cons(FS_TYPE_DIR, PERM_READ | PERM_WRITE));

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
	rcall_set("getmouse", 		wmanager_rcall_getmouse);
	rcall_set("setpanel",		wmanager_rcall_setpanel);
	rcall_set("settags",		wmanager_rcall_settags);

	rdi_set_share(wmanager_share);
	rdi_set_sync (wmanager_sync);
	rdi_set_cons (wmanager_cons);
	rdi_init_all();

	fs_plink("/sys/wmanager", RP_CONS(getpid(), 0), NULL);

	vgafd = fs_find("/dev/svga0");
	sscanf(rcall(vgafd, "getmode"), "%i %i", &width, &height);
	resize_screen(width, height);

	mousefd = fs_open("/dev/mouse");
	kbdfd   = fs_open("/dev/kbd");
	event_register(mousefd);
	event_register(kbdfd);
	event_register(vgafd);
	event_set("mouse", wmanager_mouse_event);
	event_set("key", wmanager_key_event);
	event_set("graph", wmanager_graph_event);

	current_tags = 1;

	if (fork() < 0) {
		exec("/bin/panel");
	}

	_done();

	return 0;
}
