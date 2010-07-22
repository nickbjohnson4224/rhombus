/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
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
