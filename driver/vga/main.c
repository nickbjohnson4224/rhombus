/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/driver.h>
#include <flux/ipc.h>
#include <flux/proc.h>
#include <flux/mmap.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "vga.h"

uint8_t *vmem;

int main() {
	size_t i, x, y, c;

	vmem = valloc(0x20000);
	
	for (i = 0; i < 0x20000; i += 0x1000) {
		emap(&vmem[i], 0xA0000 + i, PROT_READ | PROT_WRITE);
	}

	vga_set_mode(MODE_320x240x256);

	mode->fill(0, 0, mode->width, mode->height, 0);

	for (x = 0; x < mode->width; x++) {
		for (y = 0; y < mode->height; y++) {
			if (x < 40)       c = 0x040000;
			else if (x < 80)  c = 0x040400;
			else if (x < 120) c = 0x000400;
			else if (x < 160) c = 0x000404;
			else if (x < 200) c = 0x000004;
			else if (x < 240) c = 0x040004;
			else if (x < 280) c = 0x040404;
			else if (x < 320) c = 0x010101;
			mode->plot(x, y, (y & 0x3F) * c);
		}
	}

	mode->fill(150,  90, 190, 110, 0xFFFFFF);
	mode->fill(130, 110, 170, 130, 0xFFFFFF);
	mode->fill(150, 130, 170, 150, 0xFFFFFF);

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}
