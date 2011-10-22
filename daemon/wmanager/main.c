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

struct robject *class_window;

uint64_t vgafd, mousefd, kbdfd;
bool winkey;
int next_index = 1;

char *wmanager_rcall_listmodes(struct robject *self, uint64_t source, int argc, char **argv) {
	return strdup("any");
}

char *wmanager_rcall_createwindow(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;
	char buffer[32];

	sprintf(buffer, "/sys/wmanager/%i", next_index);
	fs_cons(buffer, "file");
	window = find_window(next_index - 1, 0);
	if (!window) {
		fprintf(stderr, "wmanager: unable to create window\n");
		return NULL;
	}
	window->owner = RP_PID(source);

	return rtoa(RP_CONS(getpid(), next_index - 1));
}

char *wmanager_rcall_setmode(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;
	int width, height;
	int old_flags;

	window = find_window(self->index, RP_PID(source));
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

char *wmanager_rcall_getmode(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	return saprintf("%i %i 32", window->width, window->height);
}

char *wmanager_rcall_syncrect(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;
	int x, y, w, h;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	if (argc != 5) return NULL;

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	w = atoi(argv[3]);
	h = atoi(argv[4]);

	update_screen(window->x + x, window->y + y, window->x + x + w, window->y + y + h);

	return strdup("T");
}

char *wmanager_rcall_unshare(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	if (!window->bitmap) return NULL;

	mutex_spin(&window->mutex);
	page_free(window->bitmap, window->width * window->height * 4);
	window->bitmap = NULL;
	mutex_free(&window->mutex);

	return strdup("T");
}

/*char *wmanager_rcall_register(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	window->flags |= LISTEN_EVENTS;

	return strdup("T");
}

char *wmanager_rcall_deregister(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	window->flags &= ~LISTEN_EVENTS;

	return strdup("T");
} */

char *wmanager_rcall_getwindowflags(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	return saprintf("%i", window->flags);
}

char *wmanager_rcall_setwindowflags(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;
	int old_flags;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	if (argc != 2) return NULL;

	old_flags = window->flags;
	window->flags = atoi(argv[1]);

	if ((old_flags & FLOATING) != (window->flags & FLOATING)) {
		update_tiling();
	}

	return strdup("T");
}

char *wmanager_rcall_getmouse(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	return saprintf("%i %i", mousex - window->x, mousey - window->y);
}

char *wmanager_rcall_setpanel(struct robject *self, uint64_t source, int argc, char **argv) {
	struct window_t *window;

	if (panel) return NULL;

	window = find_window(self->index, RP_PID(source));
	if (!window) return NULL;

	panel = window;
	panel->flags |= FLOATING;
	panel->tags = 0xffffffff;
	resize_window(panel, screen_width, panel->height, true);
	update_tiling();

	return strdup("T");
}

char *wmanager_rcall_settags(struct robject *self, uint64_t source, int argc, char **argv) {
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

int wmanager_share(struct robject *self, uint64_t source, uint8_t *buffer, size_t size, uint64_t off) {
	struct window_t *window = find_window(self->index, RP_PID(source));

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

char *wmanager_rcall_sync(struct robject *self, rp_t source, int argc, char **argv) {
	struct window_t *window = find_window(self->index, RP_PID(source));
	
	if (!window) {
		return NULL;
	}

	update_screen(window->x - 1, window->y - 1, 
		window->x + window->width + 1, window->y + window->height + 1);

	return strdup("T");
}

char *wmanager_rcall_cons(struct robject *self, rp_t source, int argc, char **argv) {
	struct robject *new_r = NULL;
	char *type;

	if (RP_PID(source) != getpid()) {
		return NULL;
	}

	if (argc == 2) {
		type = argv[1];

		if (!strcmp(type, "file")) {
			new_r = robject_cons(robject_new_index(), class_window);
			add_window(new_r->index);
		}

		if (new_r) {
			return rtoa(RP_CONS(getpid(), new_r->index));
		}
	}

	return strdup("! arg");
}

void wmanager_key_event(rp_t source, int argc, char **argv) {
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

void wmanager_mouse_event(rp_t source, int argc, char **argv) {
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

void wmanager_graph_event(rp_t source, int argc, char **argv) {
	
	if (source != vgafd) return;
	if (argc != 4) return;
	
	if (!strcmp(argv[1], "resize")) {
		resize_screen(atoi(argv[2]), atoi(argv[3]));
	}
}

int main(int argc, char **argv) {
	struct robject *root;
	int width, height;

	stdout = stderr = fopen("/dev/serial", "w");

	if (fork() < 0) {
		exec("/sbin/mouse");
	}
	mwait(PORT_CHILD, 0);

	root = rdi_dir_cons(1, ACCS_READ | ACCS_WRITE);
	robject_set_data(root, "type", (void*) "wm");
	robject_set_call(root, "createwindow", wmanager_rcall_createwindow, 0);
	robject_set_call(root, "cons",         wmanager_rcall_cons, 0);

	class_window = robject_cons(0, rdi_class_core);

	robject_set_data(class_window, "name", (void*) "class-window");
	robject_set_data(class_window, "type", (void*) "window canvas share event");

	robject_set_call(class_window, "listmodes",      wmanager_rcall_listmodes, 0);
	robject_set_call(class_window, "setmode",        wmanager_rcall_setmode,   0);
	robject_set_call(class_window, "getmode",        wmanager_rcall_getmode,   STAT_READER);
	robject_set_call(class_window, "unshare",        wmanager_rcall_unshare,   STAT_WRITER);
	robject_set_call(class_window, "getwindowflags", wmanager_rcall_getwindowflags, STAT_READER);
	robject_set_call(class_window, "setwindowflags", wmanager_rcall_setwindowflags, STAT_READER);
	robject_set_call(class_window, "syncrect",       wmanager_rcall_syncrect,  STAT_WRITER);
	robject_set_call(class_window, "getmouse",       wmanager_rcall_getmouse,  STAT_READER);
	robject_set_call(class_window, "setpanel",       wmanager_rcall_setpanel,  STAT_WRITER);
	robject_set_call(class_window, "settags",        wmanager_rcall_settags,   STAT_WRITER);
	
	rdi_global_share_hook = wmanager_share;

	fs_plink("/sys/wmanager", RP_CONS(getpid(), root->index), NULL);

	vgafd = fs_find("/dev/svga0");
	sscanf(rcall(vgafd, "getmode"), "%i %i", &width, &height);
	resize_screen(width, height);

	mousefd = fs_find("/dev/mouse");
	kbdfd   = fs_find("/dev/kbd");
	event_subscribe(mousefd);
	event_subscribe(kbdfd);
	event_subscribe(vgafd);
	event_hook("mouse", wmanager_mouse_event);
	event_hook("key",   wmanager_key_event);
	event_hook("graph", wmanager_graph_event);

	current_tags = 1;

	if (fork() < 0) {
		exec("/bin/panel");
	}

	_done();

	return 0;
}
