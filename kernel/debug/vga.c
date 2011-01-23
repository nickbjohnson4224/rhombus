/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <debug.h>
#include <stdint.h>

#if SCREEN == VGA_FULL || SCREEN == VGA_LEFT

/****************************************************************************
 * __vga_video_mem
 *
 * Region of address space containing the mmaped VGA text console memory.
 */

uint16_t *__vga_video_mem;

/****************************************************************************
 * __vga_cursor_*
 *
 * Position and attributes (current color and blink settings) of the VGA
 * cursor.
 */

size_t  __vga_cursor_base;
size_t  __vga_cursor_pos;
uint8_t __vga_cursor_attr;

#endif
