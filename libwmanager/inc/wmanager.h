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

#ifndef FLUX_WMANAGER_H
#define FLUX_WMANAGER_H

#include <stdint.h>
#include <stddef.h>

#define WMANAGER_RETURN_OK 0
#define WMANAGER_RETURN_INVALID_BITMAP_ID 1
#define WMANAGER_RETURN_INVALID_WINDOW_ID 2
#define WMANAGER_RETURN_INVALID_SIZE 3
#define WMANAGER_PORT_SET_BITMAP 32
#define WMANAGER_PORT_ADD_WINDOW (WMANAGER_PORT_SET_BITMAP + 1)
#define WMANAGER_PORT_SET_WINDOW (WMANAGER_PORT_SET_BITMAP + 2)
#define WMANAGER_PORT_DESTROY_WINDOW (WMANAGER_PORT_SET_BITMAP + 3)

int wm_init(void);

int wm_set_bitmap(uint8_t id, uint8_t *addr, size_t size);
int wm_add_window(uint8_t id, size_t width, size_t height, uint8_t bitmap);
int wm_set_window(uint8_t id, size_t width, size_t height, uint8_t bitmap);
int wm_destroy_window(uint8_t id);

#endif
