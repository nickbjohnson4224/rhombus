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
#include "svga.h"

volatile struct vbe_ctrlinfo *svga_ctrl = (void*) 0x10000;
volatile struct vbe_modeinfo *svga_mode = (void*) 0x20000;
volatile struct vbe_crtcinfo *svga_crtc = (void*) 0x30000;

int vbe_readctrl(void) {
	uint8_t image[] = {
		0xB8, 0x00, 0x4F, 	// mov ax, 0x4F00
		0xBA, 0x00, 0x10,	// mov dx, 0x1000
		0x8E, 0xC2,			// mov es, dx
		0xBF, 0x00, 0x00,	// mov di, 0x0000
		0xCD, 0x10,			// int 0x10
		0xCD, 0x30,			// int 0x30
	};

	svga_ctrl->signature[0] = 'V';
	svga_ctrl->signature[1] = 'B';
	svga_ctrl->signature[2] = 'E';
	svga_ctrl->signature[3] = '2';

	vm86_exec(image, sizeof(image));

	svga_ctrl->oem_string_ptr = (svga_ctrl->oem_string_ptr & 0xFFFF) | (svga_ctrl->oem_string_ptr >> 12);
	svga_ctrl->mode_ptr = (svga_ctrl->mode_ptr & 0xFFFF) | (svga_ctrl->mode_ptr >> 12);

	return 0;
}

int vbe_readmode(uint16_t mode) {
	uint8_t image[] = {
		0xB8, 0x01, 0x4F,	// mov ax, 0x4F01
		0xB9, 0x00, 0x00,	// mov cx, 0xXXXX
		0xBA, 0x00, 0x20,	// mov dx, 0x2000
		0x8E, 0xC2,			// mov es, dx
		0xBF, 0x00, 0x00,	// mov di, 0x0000
		0xCD, 0x10,			// int 0x10
		0xCD, 0x30,			// int 0x30
	};

	image[4] = mode & 0xFF;
	image[5] = mode >> 8;

	return vm86_exec(image, sizeof(image));
}

int vbe_setmode(uint8_t mode, uint8_t flags) {
	uint8_t image[] = {
		0xB8, 0x02, 0x4F,	// mov ax, 0x4F02
		0xBB, 0x00, 0x00,	// mov bx, 0xXXXX
		0xBA, 0x00, 0x30,	// mov dx, 0x3000
		0x8E, 0xC2,			// mov es, dx
		0xBF, 0x00, 0x00,	// mov di, 0x0000
		0xCD, 0x10,			// int 0x10
		0xCD, 0x30,			// int 0x30
	};

	image[4] = mode;
	image[5] = flags & 0xC4;
	
	return vm86_exec(image, sizeof(image));
}
