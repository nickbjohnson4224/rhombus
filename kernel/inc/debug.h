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

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <stddef.h>

/****************************************************************************
 * SCREEN
 *
 * possible settings:
 * NONE     - do not output from the kernel
 * VGA_FULL - use whole VGA text console for kernel output
 * VGA_LEFT - use (and scroll) only left half of VGA text console
 * SERIAL	- output via serial port
 */

#define NONE 		1
#define VGA_FULL	2
#define VGA_LEFT	3
#define SERIAL		4

#define SCREEN SERIAL

/* debug screen state ******************************************************/

#if (SCREEN == VGA_FULL) || (SCREEN == VGA_LEFT)

	extern uint16_t *__vga_video_mem;

	extern size_t  __vga_cursor_base;
	extern size_t  __vga_cursor_pos;
	extern uint8_t __vga_cursor_attr;

#endif

/* debug screen operations *************************************************/

void debug_init  (void);
void debug_clear (void);
void debug_scroll(size_t lines);
void debug_char  (char c);
void debug_string(const char *s);
void debug_color (uint32_t color);

#if (SCREEN == NONE) || (SCREEN == SERIAL)

	#define COLOR_BLACK		0x00
	#define COLOR_DBLUE		0x00
	#define COLOR_DGREEN	0x00
	#define COLOR_DCYAN		0x00
	#define COLOR_DRED		0x00
	#define COLOR_DMAGENTA	0x00
	#define COLOR_DORANGE	0x00
	#define COLOR_LGRAY		0x00
	#define COLOR_DGRAY		0x00
	#define COLOR_BLUE		0x00
	#define COLOR_GREEN		0x00
	#define COLOR_CYAN		0x00
	#define COLOR_RED		0x00
	#define COLOR_MAGENTA	0x00
	#define COLOR_ORANGE	0x00
	#define COLOR_WHITE 	0x00	

#endif

#if (SCREEN == VGA_FULL) || (SCREEN == VGA_LEFT)

	#define COLOR_BLACK		0x00
	#define COLOR_DBLUE		0x01
	#define COLOR_DGREEN	0x02
	#define COLOR_DCYAN		0x03
	#define COLOR_DRED		0x04
	#define COLOR_DMAGENTA	0x05
	#define COLOR_DORANGE	0x06
	#define COLOR_LGRAY		0x07
	#define COLOR_DGRAY		0x08
	#define COLOR_BLUE		0x09
	#define COLOR_GREEN		0x0A
	#define COLOR_CYAN		0x0B
	#define COLOR_RED		0x0C
	#define COLOR_MAGENTA	0x0D
	#define COLOR_ORANGE	0x0E
	#define COLOR_WHITE 	0x0F

#endif

/* high level debugging output *********************************************/

void __itoa(char *buffer, size_t number, size_t base);

void debug_printf(const char *fmt, ...);
void debug_panic (const char *message);
void debug_dumpi (uintptr_t *base, int count);

/* debugger - TODO *********************************************************/

#endif/*DEBUG_H*/
