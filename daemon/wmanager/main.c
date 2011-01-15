/*
 * Copyright (C) 2011 Jaagup Repan
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <exec.h>
#include <ipc.h>
#include <natio.h>
#include <proc.h>
#include <page.h>
#include <wmanager.h>

#define LIST_ADD(item) \
{ \
	struct item##_t **ptr = &item##s; \
	while (*ptr) { \
		item->prev = *ptr; \
		ptr = &(*ptr)->next; \
	} \
	*ptr = item; \
}

#define LIST_REMOVE(item) \
{ \
	if (item->prev) { \
		item->prev->next = item->next; \
	} \
	else { \
		item ## s = item->next; \
	} \
	if (item->next) { \
		item->next->prev = item->prev; \
	} \
	free(item); \
}

#define LIST_FIND(list, owner, id) \
{ \
	struct list##_t *item = list##s; \
	while (item) { \
		if (item->owner == owner && item->id == id) { \
			return item; \
		} \
		item = item->next; \
	} \
	return NULL; \
}

#define LIST_FREE(list) \
{ \
	struct list##_t *item = list##s; \
	while (item) { \
		struct list##_t *next = item->next; \
		free(item); \
		item = next; \
	} \
}

struct window_t {
	uint32_t owner;
	uint8_t id;
	size_t x, y;
	size_t width, height;
	uint8_t bitmap;
	struct window_t *prev, *next;
};
struct bitmap_t {
	uint32_t owner;
	uint8_t id;
	uint8_t *address;
	size_t size;
	struct bitmap_t *prev, *next;
};

uint8_t *screen;
size_t screen_width, screen_height;
struct window_t *windows;
struct bitmap_t *bitmaps;

void return_message(int ret, int target) {
	struct msg *msg = malloc(sizeof(struct msg));
	msg->count = 1;
	msg->packet = aalloc(msg->count * PAGESZ, PAGESZ);
	*((uint32_t*) msg->packet) = ret;
	msend(PORT_REPLY, target, msg);
}

struct window_t *find_window(uint32_t owner, uint8_t id) LIST_FIND(window, owner, id)
struct bitmap_t *find_bitmap(uint32_t owner, uint8_t id) LIST_FIND(bitmap, owner, id)

void add_window(struct msg *msg) {
	static int tmp = 0;
	uint8_t id    = ((uint32_t *) msg->packet)[0];
	size_t width  = ((uint32_t *) msg->packet)[1];
	size_t height = ((uint32_t *) msg->packet)[2];
	size_t bitmap = ((uint32_t *) msg->packet)[3];

	if (find_window(msg->source, id)) {
		return_message(WMANAGER_RETURN_INVALID_WINDOW_ID, msg->source);
		return;
	}
	if (!find_bitmap(msg->source, id)) {
		return_message(WMANAGER_RETURN_INVALID_BITMAP_ID, msg->source);
		return;
	}
	if (find_bitmap(msg->source, id)->size < width * height * 4) {
		return_message(WMANAGER_RETURN_INVALID_SIZE, msg->source);
		return;
	}

	struct window_t *window = malloc(sizeof(struct window_t));
	window->owner = msg->source;
	window->id = id;
	window->x = tmp;
	window->y = tmp;
	tmp += 15;
	window->width = width;
	window->height = height;
	window->bitmap = bitmap;
	window->next = window->prev = NULL;

	LIST_ADD(window)
	return_message(WMANAGER_RETURN_OK, msg->source);
}

void set_window(struct msg *msg) {
	uint8_t id    = ((uint32_t *) msg->packet)[0];
	size_t width  = ((uint32_t *) msg->packet)[1];
	size_t height = ((uint32_t *) msg->packet)[2];
	size_t bitmap = ((uint32_t *) msg->packet)[3];
	struct window_t *window = find_window(msg->source, id);

	if (!window) {
		return_message(WMANAGER_RETURN_INVALID_WINDOW_ID, msg->source);
		return;
	}
	if (!find_bitmap(msg->source, id)) {
		return_message(WMANAGER_RETURN_INVALID_BITMAP_ID, msg->source);
		return;
	}
	if (find_bitmap(msg->source, id)->size < width * height * 4) {
		return_message(WMANAGER_RETURN_INVALID_SIZE, msg->source);
		return;
	}

	window->width = width;
	window->height = height;
	window->bitmap = bitmap;

	return_message(WMANAGER_RETURN_OK, msg->source);
}

void destroy_window(struct msg *msg) {
	uint8_t id = *(uint32_t*) msg->packet;
	struct window_t *window = find_window(msg->source, id);
	if (!window) {
		return_message(WMANAGER_RETURN_INVALID_WINDOW_ID, msg->source);
		return;
	}
	LIST_REMOVE(window)
	return_message(WMANAGER_RETURN_OK, msg->source);
}

void set_bitmap(struct msg *msg) {
	uint8_t id = *(uint8_t*) msg->packet;
	struct bitmap_t *bitmap = find_bitmap(msg->source, id);
	if (bitmap) {
		if (msg->packet) {
			bitmap->address = msg->packet;
			bitmap->size = msg->count * PAGESZ;
		}
		else {
			LIST_REMOVE(bitmap)
		}
	}
	else {
		/* Add bitmap */
		bitmap = malloc(sizeof(struct bitmap_t));
		bitmap->id = id;
		bitmap->owner = msg->source;
		bitmap->address = msg->packet;
		bitmap->prev = bitmap->next = NULL;
		bitmap->size = msg->count * PAGESZ;
		LIST_ADD(bitmap)
	}
	return_message(WMANAGER_RETURN_OK, msg->source);
}

int main(int argc, char **argv) {
	stdout = stderr = fopen("/dev/serial", "w");

	if (fork() < 0) {
		exec("/sbin/vga");
	}
	mwaits(PORT_CHILD, 0);

	when(WMANAGER_PORT_SET_BITMAP, set_bitmap);
	when(WMANAGER_PORT_ADD_WINDOW, add_window);
	when(WMANAGER_PORT_SET_WINDOW, set_window);
	when(WMANAGER_PORT_DESTROY_WINDOW, destroy_window);
	io_link("/sys/wmanager", RP_CONS(getpid(), 0));

	FILE *vga = fopen("/dev/vga0", "r");
	fscanf(vga, "%i %i", &screen_width, &screen_height);
	fclose(vga);
	screen = malloc(screen_width * screen_height * 3);
	uint64_t vgafd = io_find("/dev/vga0");
	mmap(vgafd, screen, screen_width * screen_height * 3, 0, PROT_READ);

	if (fork() < 0) {
		exec("/bin/testapp");
	}

	while (1) {
		memset(screen, 0, screen_width * screen_height * 3);
		struct window_t *window = windows;
		while (window) {
			/* content */
			struct bitmap_t *bitmap = find_bitmap(window->owner, window->id);
			if (bitmap && bitmap->size >= window->width * window->height * 4) {
				for (size_t x = window->x; (x < (window->x + window->width)) && (x < screen_width); x++) {
					for (size_t y = window->y; (y < (window->y + window->height)) && (y < screen_height); y++) {
						size_t screen_index = (x + y * screen_width) * 3;
						size_t window_index = ((x - window->x) + (y - window->y) * window->width) * 4;
						double alpha = bitmap->address[window_index + 3] / 255.0;
						for (int c = 0; c < 3; c++) {
							screen[screen_index + c] = (1 - alpha) * screen[screen_index + c] +
								alpha * bitmap->address[window_index + c];
						}
					}
				}
			}
			
			/* decorations */
			for (size_t x = window->x >= 1 ? window->x - 1 : 0;
					(x < (window->x + window->width + 1)) && (x < screen_width); x++) {
				if (window->y >= 1) {
					for (int c = 0; c < 3; c++) {
						screen[(x + (window->y - 1) * screen_width) * 3 + c] = 0xff;
					}
				}
				if (window->y + window->height < screen_height) {
					for (int c = 0; c < 3; c++) {
						screen[(x + (window->y + window->height) * screen_width) * 3 + c] = 0xff;
					}
				}
			}
			for (size_t y = window->y; (y < (window->y + window->height)) && (y < screen_height); y++) {
				if (window->x >= 1) {
					for (int c = 0; c < 3; c++) {
						screen[(window->x - 1 + y * screen_width) * 3 + c] = 0xff;
					}
				}
				if (window->x + window->width < screen_width) {
					for (int c = 0; c < 3; c++) {
						screen[(window->x + window->width + y * screen_width) * 3 + c] = 0xff;
					}
				}
			}

			window = window->next;
		}
		sync(vgafd);
	}

	free(screen);
	LIST_FREE(bitmap)
	LIST_FREE(window)
	return 0;
}
