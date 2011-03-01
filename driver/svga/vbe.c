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

#include <string.h>
#include <stdlib.h>
#include <driver.h>
#include "svga.h"

struct vbe_ctrlinfo *svga_ctrl = (void*) 0x10000;
struct vbe_modeinfo *svga_mode = (void*) 0x20000;
struct vbe_crtcinfo *svga_crtc = (void*) 0x30000;

int vbe_readctrl(void) {
	memcpy(svga_ctrl->signature, "VBE2", 4);

	return vbe_cmd(0, 0, 0, 0, (uintptr_t) svga_ctrl);
}

int vbe_readmode(uint16_t mode) {
	return vbe_cmd(1, 0, mode, 0, (uintptr_t) svga_mode);
}

int vbe_setmode(uint16_t mode, uint8_t flags) {
	return vbe_cmd(2, mode | flags << 9, 0, 0, (uintptr_t) svga_crtc);
}

int vbe_setbank(uint8_t window, uint16_t bank) {
	return vbe_cmd(5, window, 0, bank, 0);
}

int vbe_cmd(uint8_t num, uint16_t bx, uint16_t cx, uint16_t dx, uint32_t esdi) {
	uint8_t image[] = {
		0xB8, 0x00, 0x4F,	// mov ax, 0x4F??
		0xBB, 0x00, 0x00,	// mov bx, 0x????
		0xB9, 0x00, 0x00,	// mov cx, 0x????
		0xBA, 0x00, 0x00,	// mov dx, 0x????
		0xBF, 0x00, 0x00,	// mov di, 0x????
		0x8E, 0xC2,			// mov es, dx
		0xBA, 0x00, 0x00,	// mov dx, 0x????
		0xCD, 0x10,			// int 0x10
		0xBf, 0x00, 0x00,	// mov di, 0x0000
		0xBA, 0x00, 0x70,	// mov dx, 0x7000
		0x8E, 0xC2,			// mov es, dx
		0x26, 0x88, 0x25,	// mov [es:di], ah
		0xCD, 0x30,			// int 0x30
	};

	// patch image
	image[1]  = num;
	image[4]  = bx & 0xFF;
	image[5]  = bx >> 8;
	image[7]  = cx & 0xFF;
	image[8]  = cx >> 8;
	image[10] = 0;
	image[11] = (esdi >> 12) & 0xF0;
	image[13] = esdi & 0xFF;
	image[14] = esdi >> 8;
	image[18] = dx & 0xFF;
	image[19] = dx >> 8;

	vm86_exec(image, sizeof(image));

	return *((uint8_t*) 0x70000);
}
