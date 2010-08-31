/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <debug.h>
#include <stdint.h>

/****************************************************************************
 * debug_char
 *
 * Prints a single character <c> onto debugging output. Non-printing 
 * characters, such as tab and backspace, are handled.
 */

void debug_char(char c) {

	#if (SCREEN == VGA_FULL) || (SCREEN == VGA_LEFT)

	#if SCREEN == VGA_LEFT
		if (__vga_cursor_pos % 80 < 40) {
			__vga_cursor_pos += 40;
		}
	#endif
	
	switch (c) {
		case '\t': 
			__vga_cursor_pos -= __vga_cursor_pos % 4;
			__vga_cursor_pos += 4;
			break;
		case '\n':
			__vga_cursor_pos -= __vga_cursor_pos % 80;
			__vga_cursor_pos += 80;
			__vga_cursor_base = __vga_cursor_pos;

			if (__vga_cursor_pos >= 1920) {
				debug_scroll(1);
			}
			break;
		case '\r': 
			__vga_cursor_pos -= __vga_cursor_pos % 80; 
			__vga_cursor_base = __vga_cursor_pos;
			break;
		case '\b': 
			if (__vga_cursor_base < __vga_cursor_pos) {
				__vga_cursor_pos--;
			}
			
			__vga_video_mem[__vga_cursor_pos] = (__vga_cursor_attr << 8) | ' ';
			break;
		default:
			__vga_video_mem[__vga_cursor_pos] = (__vga_cursor_attr << 8) | c;
			__vga_cursor_pos++;
			break;
	}

	if (__vga_cursor_pos >= 2000) {
		debug_scroll(1);
	}
	#endif
}
