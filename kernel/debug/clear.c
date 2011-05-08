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
#include <string.h>

/****************************************************************************
 * debug_clear
 *
 * Clears debugging output.
 */

void debug_clear(void) {
	#if (SCREEN == VGA_FULL) || (SCREEN == VGA_LEFT)
	size_t i, j;
	#endif
	
	#if SCREEN == VGA_FULL
		for (i = 0; i < 25; i++) {
			for (j = 0; j < 80; j++) {
				__vga_video_mem[i*80+j] = (__vga_cursor_attr << 8) | ' ';
			}
		}
	#endif
	#if SCREEN == VGA_LEFT
		for (i = 0; i < 25; i++) {
			for (j = 40; j < 80; j++) {
				__vga_video_mem[i*80+j] = (__vga_cursor_attr << 8) | ' ';
			}
		}
	#endif

}
