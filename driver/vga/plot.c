/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdint.h>
#include <driver.h>

#include "vga.h"

void vga_linear_plot(size_t x, size_t y, uint32_t c) {
	size_t off;

	if (y > mode->height) return;
	if (x > mode->width)  return;

	off = mode->offset + x + y * mode->width;
	vmem[off] = mode->get_color(c);
}

void vga_planar_plot(size_t x, size_t y, uint32_t c) {
	size_t off, plane;

	if (y > mode->height) return;
	if (x > mode->width)  return;

	off   = mode->offset + (x + y * mode->width) / 4;
	plane = x & 3;

	outb(GC_INDEX, 4);
	outb(GC_DATA, plane);
	outb(SEQ_INDEX, 2);
	outb(SEQ_DATA, 1 << plane);

	vmem[off] = mode->get_color(c);
}
