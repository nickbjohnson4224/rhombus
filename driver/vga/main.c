/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <driver.h>
#include <ipc.h>
#include <proc.h>
#include <mmap.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "vga.h"

uint8_t *vmem;

int main(int argc, char **argv) {
	size_t i, x, y, c, cx, cy;

	vmem = valloc(0x20000);
	
	for (i = 0; i < 0x20000; i += 0x1000) {
		emap(&vmem[i], 0xA0000 + i, PROT_READ | PROT_WRITE);
	}

	if (argc <= 1) {
		vga_set_mode(MODE_320x240x256);
	}
	else {
		vga_set_mode(atoi(argv[1]));
	}

	mode->fill(0, 0, mode->width, mode->height, 0x2211BB);

	cx = mode->width  / 2;
	cy = mode->height / 2;

	mode->fill(cx - 11, cy - 31, cx + 31, cy -  9, 0x000000);
	mode->fill(cx - 31, cy - 11, cx + 11, cy + 11, 0x000000);
	mode->fill(cx - 11, cy + 11, cx + 11, cy + 31, 0x000000);

	mode->fill(cx - 10, cy - 30, cx + 30, cy - 10, 0xFFFFFF);
	mode->fill(cx - 30, cy - 10, cx + 10, cy + 10, 0xFFFFFF);
	mode->fill(cx - 10, cy + 10, cx + 10, cy + 30, 0xFFFFFF);

	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
