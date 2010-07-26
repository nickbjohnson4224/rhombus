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

	mode->fill(0, 0, mode->width, mode->height, 0);

	for (x = 0; x < mode->width; x++) {
		for (y = 0; y < mode->height; y++) {
			c = 0;
			if (x < 40)       c = 0x040000 | 0x000100 * ((x - 0)   / 10);
			else if (x < 80)  c = 0x000400 | 0x010000 * (((79 - x)  / 10) + 1);
			else if (x < 120) c = 0x000400 | 0x000001 * ((x - 80)  / 10);
			else if (x < 160) c = 0x000004 | 0x000100 * (((159 - x) / 10) + 1);
			else if (x < 200) c = 0x000004 | 0x010000 * ((x - 160) / 10);
			else if (x < 240) c = 0x040000 | 0x000001 * (((239 - x) / 10) + 1);
			else if (x < 320) c = 0x040404;
			mode->plot(x, y, ((y / 2) & 0x3F) * c);
		}
	}

	cx = mode->width  / 2;
	cy = mode->height / 2;

	mode->fill(cx - 10, cy - 30, cx + 30, cy - 10, 0xFFFFFF);
	mode->fill(cx - 30, cy - 10, cx + 10, cy + 10, 0xFFFFFF);
	mode->fill(cx - 10, cy + 10, cx + 10, cy + 30, 0xFFFFFF);

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}
