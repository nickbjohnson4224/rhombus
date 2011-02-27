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

const size_t width = 256;
const size_t height = 30;
const size_t size = width * height * 4;
uint8_t *bitmap;
uint64_t wID;

void testapp_event(uint64_t source, uint64_t event) {
	printf("testapp event\n");
}

void draw(uint8_t alpha) {
	for (size_t i = 3; i <= size; i += 4)	{
		bitmap[i] = alpha;
	}
	wm_update(wID);
}

int main(int argc, char **argv) {
	event_register(io_find("/sys/wmanager"), testapp_event);

	bitmap = malloc(size);
	memset(bitmap, 0, size);
	for (size_t i = 0; i <= 0xff; i++)	{
		for (size_t line = 0; line < 3; line++) {
			for (size_t j = 0; j < 10; j++) {
				for (size_t c = 0; c < 3; c++) {
					bitmap[(i + (10 * line + j) * width) * 4 + c] = line == c ? i : 0;
				}
			}
		}
	}

	wID = wm_create_window(width, height);
	wm_set_bitmap(wID, bitmap, size);
	while (1) {
		for (int alpha = 0; alpha <= 0xff; alpha += 0x10) {
			draw(alpha);
		}
		for (int alpha = 0xfe; alpha >= 0; alpha -= 0x10) {
			draw(alpha);
		}
	}

	wm_destroy_window(wID);
	free(bitmap);
	return 0;
}
