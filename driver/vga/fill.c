/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details.
 */

#include <stdint.h>
#include <driver.h>

#include "vga.h"

void vga_linear_fill(size_t x0, size_t y0, size_t x1, size_t y1, uint32_t c) {
	size_t x, y, off;
	uint8_t color;

	color = mode->get_color(c);

	for (y = y0; y < y1; y++) {
		for (x = x0; x < x1; x++) {
			off = mode->offset + x + y * mode->width;
			vmem[off] = color;
		}
	}
}

void vga_planar_fill(size_t x0, size_t y0, size_t x1, size_t y1, uint32_t c) {
	size_t x, y, off, p;
	uint8_t color;

	color = mode->get_color(c);

	for (p = 0; p < 4; p++) {
		outb(GC_INDEX, 4);
		outb(GC_DATA, p);
		outb(SEQ_INDEX, 2);
		outb(SEQ_DATA, 1 << p);

		for (x = (x0 & ~0x3) + p; x < (x1 & ~0x3) + 4; x += 4) {
			if (x < x0 || x > x1) continue;
			for (y = y0; y < y1; y++) {
				off = mode->offset + (x + y * mode->width) / 4;
				vmem[off] = color;
			}
		}
	}
}
