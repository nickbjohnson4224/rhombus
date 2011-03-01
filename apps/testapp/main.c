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

#include <wmanager.h>
#include <stdlib.h>
#include <ipc.h>
#include <page.h>
#include <string.h>
#include <stdio.h>
#include <natio.h>

size_t size;
uint8_t *bitmap;
uint64_t wID;

void resize(size_t width, size_t height) {
	size = width * height * 4;
	bitmap = realloc(bitmap, size);
	for (size_t i = 0; i <= width; i++)	{
		for (size_t line = 0; line < 3; line++) {
			for (size_t j = 0; j < height / 3; j++) {
				for (size_t c = 0; c < 3; c++) {
					bitmap[(i + ((height / 3) * line + j) * width) * 4 + c] = line == c ? (255.0 / width * i) : 0;
				}
			}
		}
	}
	wm_set_bitmap(wID, bitmap, size);
}

void testapp_event(uint64_t source, uint64_t value) {
	int type = value >> 62;
	int data = value & ~(0x3LL << 62);
	if (type == 0x0) { // resize
		resize(data >> 16, data & 0xffff);
	}
}

void draw(uint8_t alpha) {
	for (size_t i = 3; i <= size; i += 4)	{
		bitmap[i] = alpha;
	}
	wm_update(wID);
}

int main(int argc, char **argv) {
	wID = wm_create_window(256, 30);
	event_register(wID, testapp_event);
	resize(256, 30);

	while (1) {
		for (int alpha = 0; alpha <= 0xff; alpha++) {
			draw(alpha);
		}
		for (int alpha = 0xfe; alpha >= 0; alpha--) {
			draw(alpha);
		}
	}

	wm_destroy_window(wID);
	free(bitmap);
	return 0;
}
