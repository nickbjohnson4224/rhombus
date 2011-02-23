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
#include <stdlib.h>
#include "list.h"

struct bitmap_t *bitmaps;

int add_bitmap(uint32_t id) {
	struct bitmap_t *bitmap;
	LIST_FIND(bitmap, bitmap, item->id == id)
	if (bitmap) {
		return -1;
	}

	bitmap = malloc(sizeof(struct bitmap_t));
	bitmap->id = id;
	bitmap->address = NULL;
	bitmap->size = 0;
	bitmap->prev = bitmap->next = NULL;

	LIST_ADD(bitmap);
	return 0;
}

int remove_bitmap(uint32_t id) {
	struct bitmap_t *bitmap;
	LIST_FIND(bitmap, bitmap, item->id == id)
	if (!bitmap) {
		return -1;
	}
	LIST_REMOVE(bitmap)
	return 0;
}

int set_bitmap(uint32_t id, uint8_t *address, size_t size) {
	struct bitmap_t *bitmap;
	LIST_FIND(bitmap, bitmap, item->id == id)
	if (!bitmap) {
		return -1;
	}
	if (bitmap->address) {
		return -1;
	}
	bitmap->address = address;
	bitmap->size = size;
	return 0;
}

void blit_bitmap(const uint8_t *bitmap, int tox, int toy, size_t width, size_t height) {
	for (size_t x = tox >= 0 ? tox : 0; x < tox + width && x < screen_width; x++) {
		for (size_t y = toy >= 0 ? toy : 0; y < toy + height && y < screen_height; y++) {
			size_t screen_index = (x + y * screen_width) * 3;
			size_t bitmap_index = ((x - tox) + (y - toy) * width) * 4;
			double alpha = bitmap[bitmap_index + 3] / 255.0;
			for (int c = 0; c < 3; c++) {
				screen[screen_index + c] = (1 - alpha) * screen[screen_index + c] +
					alpha * bitmap[bitmap_index + c];
			}
		}
	}
}
