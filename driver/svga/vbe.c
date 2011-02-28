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

struct vbe_ctrlinfo *svga_controller;
struct vbe_modeinfo **svga_modev;

int vbe_readctrl(void) {
	uint8_t image[] = {
		0xB8, 0x00, 0x4F, 	// mov ax, 0x4F00
		0xBA, 0x00, 0x10,	// mov dx, 0x1000
		0x8E, 0xC2,			// mov es, dx
		0xBF, 0x00, 0x00,	// mov di, 0x0000
		0xCD, 0x10,			// int 0x10
		0xBA, 0x00, 0x20,	// mov dx, 0x2000
		0x8E, 0xC2,			// mov es, dx
		0x26, 0x89, 0x05,	// mov [es:di], ax
		0xCD, 0x30,			// int 0x30
	};

	vm86_setup();

	svga_controller = (void*) 0x10000;
	svga_controller->signature[0] = 'V';
	svga_controller->signature[1] = 'B';
	svga_controller->signature[2] = 'E';
	svga_controller->signature[3] = '2';

	return vm86_exec(image, sizeof(image));
}
