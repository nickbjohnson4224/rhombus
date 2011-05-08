/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <arch.h>
#include <debug.h>
#include <ports.h>

/****************************************************************************
 * debug_init
 *
 * Initializes debugging output.
 */

void debug_init(void) {

	#if SCREEN == SERIAL
		outb(0x3F9, 0x00);
		outb(0x3FB, 0x80);
		outb(0x3F8, 0x03);
		outb(0x3F9, 0x00);
		outb(0x3FB, 0x03);
		outb(0x3FA, 0xC7);
		outb(0x3FC, 0x0B);
	#endif

	#if (SCREEN == VGA_FULL) || (SCREEN == VGA_LEFT)
		__vga_video_mem = (void*) (KSPACE + 0xB8000);
		
		__vga_cursor_base = 0;
		__vga_cursor_pos  = 0;
		__vga_cursor_attr = (COLOR_BLACK << 4) | COLOR_WHITE;
	#endif

	debug_clear();
}
