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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
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
		item##s = item->next; \
	} \
	if (item->next) { \
		item->next->prev = item->prev; \
	} \
	free(item); \
}

#define LIST_FIND(list, id) \
{ \
	struct list##_t *item = list##s; \
	while (item) { \
		if (item->id == id) { \
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
	uint32_t id;
	size_t x, y;
	size_t width, height;
	uint32_t bitmap;
	struct window_t *prev, *next;
};
struct bitmap_t {
	uint32_t id;
	uint8_t *address;
	size_t size;
	struct bitmap_t *prev, *next;
};

const uint32_t bitmaps_dir = 1, windows_dir = 2;
uint8_t *screen;
size_t screen_width, screen_height;
struct window_t *windows;
struct bitmap_t *bitmaps;
uint64_t vgafd;

struct window_t *find_window(uint32_t id) LIST_FIND(window, id)
struct bitmap_t *find_bitmap(uint32_t id) LIST_FIND(bitmap, id)

int wmanager_push(struct vfs_obj *file) {
	static int tmp = 0;

	if (file->index == bitmaps_dir || file->index == windows_dir) {
		return 0;
	}

	else if (file->mother->index == windows_dir) {
		if (find_window(file->index)) {
			return -1;
		}

		struct window_t *window = malloc(sizeof(struct window_t));
		window->id = file->index;
		window->x = tmp;
		window->y = tmp;
		tmp += 15;
		window->width = window->height = window->bitmap = 0;
		window->next = window->prev = NULL;

		LIST_ADD(window)
	}

	else if (file->mother->index == bitmaps_dir) {
		if (find_bitmap(file->index)) {
			return -1;
		}

		struct bitmap_t *bitmap = malloc(sizeof(struct bitmap_t));
		bitmap->id = file->index;
		bitmap->address = NULL;
		bitmap->size = 0;
		bitmap->prev = bitmap->next = NULL;

		LIST_ADD(bitmap);
	}

	else {
		return -1;
	}

	return 0;
}

char *wmanager_rcall(struct vfs_obj *file, const char *args) {
	struct window_t *window = find_window(file->index);

	if (!window) {
		return NULL;
	}

	if (strlen(args) <= 1) {
		return NULL;
	}

	if (args[0] == 's') { // size
		size_t width, height;
		if (sscanf(args + 2, "%i %i", &width, &height) != 2) {
			return NULL;
		}
		if (find_bitmap(window->bitmap) && find_bitmap(window->bitmap)->size != width * height * 4) {
			return NULL;
		}
		window->width = width;
		window->height = height;
	}

	else  if (args[0] == 'b') { //bitmap
		uint32_t bitmap;
		if (sscanf(args + 2, "%i", &bitmap) != 1) {
			return NULL;
		}
		if (!find_bitmap(bitmap)) {
			return NULL;
		}
		if (find_bitmap(bitmap)->size != window->width * window->height * 4) {
			return NULL;
		}
		window->bitmap = bitmap;
	}

	else {
		return NULL;
	}

	return strdup("ok");
}

int wmanager_pull(struct vfs_obj *file) {
	if (file->mother->index == windows_dir) {
		struct window_t *window = find_window(file->index);
		if (!window) {
			return -1;
		}
		LIST_REMOVE(window)
	}

	else if (file->mother->index == bitmaps_dir) {
		struct bitmap_t *bitmap = find_bitmap(file->index);
		if (!bitmap) {
			return -1;
		}
		LIST_REMOVE(bitmap)
	}

	else {
		return -1;
	}

	return 0;
}

int wmanager_mmap(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off) {
	struct bitmap_t *bitmap = find_bitmap(file->index);
	if (off != 0) {
		return -1;
	}
	if (!bitmap) {
		return -1;
	}
	if (bitmap->address) {
		return -1;
	}
	bitmap->address = buffer;
	bitmap->size = size;
	return 0;
}

int wmanager_sync(struct vfs_obj *file) {
	memset(screen, 0, screen_width * screen_height * 3);
	struct window_t *window = windows;
	while (window) {
		/* content */
		struct bitmap_t *bitmap = find_bitmap(window->bitmap);
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
	return 0;
}

struct vfs_obj *wmanager_cons(int type) {
	static int next_index = 1;
	struct vfs_obj *fobj = NULL;

	switch (type) {
	case RP_TYPE_FILE:
	case RP_TYPE_DIR:
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

//todo: owner control
int main(int argc, char **argv) {
	struct vfs_obj *root;
	char *buffer;

	stdout = stderr = fopen("/dev/serial", "w");

	if (fork() < 0) {
		exec("/sbin/vga");
	}
	mwait(PORT_CHILD, 0);

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = RP_TYPE_DIR;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, PERM_READ | PERM_WRITE);
	vfs_set_index(0, root);

	di_wrap_share(wmanager_mmap);
	di_wrap_rcall(wmanager_rcall);
	di_wrap_sync(wmanager_sync);
	vfs_wrap_cons(wmanager_cons);
	vfs_wrap_push(wmanager_push);
	vfs_wrap_pull(wmanager_pull);
	vfs_wrap_init();

	io_link("/sys/wmanager", RP_CONS(getpid(), 0));
	io_cons("/sys/wmanager/bitmaps", RP_TYPE_DIR);
	io_cons("/sys/wmanager/windows", RP_TYPE_DIR);

	vgafd = io_find("/dev/vga0");
	buffer = rcall(vgafd, "dim");
	sscanf(buffer, "%i %i", &screen_width, &screen_height);
	screen = malloc(screen_width * screen_height * 3);
	share(vgafd, screen, screen_width * screen_height * 3, 0, PROT_READ);

	if (fork() < 0) {
		exec("/bin/testapp");
	}

	_done();

	free(screen);
	LIST_FREE(bitmap)
	LIST_FREE(window)
	return 0;
}
