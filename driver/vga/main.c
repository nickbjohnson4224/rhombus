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

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <driver.h>
#include <proc.h>
#include <page.h>
#include <ipc.h>

#include "vga.h"

uint8_t *vmem;

int main(int argc, char **argv) {
	size_t i, x, cx, cy;
	uint32_t color;

	vmem = valloc(0x20000);
	
	page_phys(vmem, 0x20000, PROT_READ | PROT_WRITE, 0xA0000);

	if (argc <= 1) {
		vga_set_mode(MODE_320x240x256);
	}
	else {
		vga_set_mode(atoi(argv[1]));
	}

	mode->fill(0, 0, mode->width, mode->height, 0x2211BB);

	for (x = 0; x < mode->width; x++) {
		i = x % 0x60;
		if      (i >= 0x01 && i < 0x10) color = 0xFF0000 + (i - 0x00) * 0x001100;
		else if (i >= 0x10 && i < 0x20) color = 0x00FF00 + (0x1F - i) * 0x110000;
		else if (i >= 0x20 && i < 0x30) color = 0x00FF00 + (i - 0x20) * 0x000011;
		else if (i >= 0x30 && i < 0x40) color = 0x0000FF + (0x3F - i) * 0x001100;
		else if (i >= 0x40 && i < 0x50) color = 0x0000FF + (i - 0x40) * 0x110000;
		else if (i >= 0x50 && i < 0x60) color = 0xFF0000 + (0x5F - i) * 0x000011;
		mode->fill(x * 2, 0, x * 2 + 2, mode->height, color);
	}

	cx = mode->width  / 2;
	cy = mode->height / 2;

/*	mode->fill(cx - 11, cy - 31, cx + 31, cy -  9, 0x000000);
	mode->fill(cx - 31, cy - 11, cx + 11, cy + 11, 0x000000);
	mode->fill(cx - 11, cy + 11, cx + 11, cy + 31, 0x000000);

	mode->fill(cx - 10, cy - 30, cx + 30, cy - 10, 0xFFFFFF);
	mode->fill(cx - 30, cy - 10, cx + 10, cy + 10, 0xFFFFFF);
	mode->fill(cx - 10, cy + 10, cx + 10, cy + 30, 0xFFFFFF); */

	mode->fill(cx - 30, cy - 30, cx + 30, cy + 30, 0xFFFFFF);
	
	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
