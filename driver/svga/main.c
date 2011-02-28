/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <driver.h>
#include <stdio.h>
#include "svga.h"

int main(int argc, char **argv) {
	int i, mode;
	uint16_t *modev;

	vm86_setup();
	vbe_readctrl();

	printf("signature: %c%c%c%c\n", 
		svga_ctrl->signature[0], 
		svga_ctrl->signature[1], 
		svga_ctrl->signature[2], 
		svga_ctrl->signature[3]);	

	printf("VBE version: %x.%x\n", svga_ctrl->version >> 8, svga_ctrl->version & 0xFF);
	printf("OEM string: %s\n", svga_ctrl->oem_string_ptr);
	printf("capabilities: %x %x %x %x\n",
		svga_ctrl->caps[0],
		svga_ctrl->caps[1],
		svga_ctrl->caps[2],
		svga_ctrl->caps[3]);

	modev = (void*) svga_ctrl->mode_ptr;
	for (i = 0; modev[i] != 0xFFFF && i < 10; i++) {
		vbe_readmode(modev[i]);

		printf("mode: %d %d:%d:%d\n", modev[i], svga_mode->xres, svga_mode->yres, svga_mode->depth);
	}

	printf("memory size: %x KB\n", svga_ctrl->memory * 64);

	printf("\n");
	printf("choose a mode: ");
	scanf("%i", &mode);

	vbe_setmode(mode, 0);

	return 0;
}
