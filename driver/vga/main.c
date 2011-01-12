/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

size_t vga_read(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	char data[16];

	sprintf(data, "%u %u", mode->width, mode->height);

	if (strlen(data) >= size) {
		return 0;
	}

	memcpy(buffer, data, strlen(data) + 1);
	return strlen(data);
}

size_t vga_write(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	int off = offset;
	size_t i;
	uint32_t pixel;

	for (i = 0; i < size / 3; i++) {
		pixel = (buffer[i * 3] << 16) | (buffer[i * 3 + 1] << 8) | buffer[i * 3 + 2];
		mode->plot((i + off) % mode->width, (i + off) / mode->width, pixel);
	}

	return size;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = FOBJ_FILE;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, FS_PERM_READ | FS_PERM_WRITE);
	vfs_set_index(0, root);

	vmem = valloc(0x20000);
	page_phys(vmem, 0x20000, PROT_READ | PROT_WRITE, 0xA0000);

	vga_set_mode(MODE_320x200x256);

	mode->fill(0, 0, mode->width, mode->height, 0);

	/* set up driver interface */
	di_wrap_read (vga_read);
	di_wrap_write(vga_write);
	vfs_wrap_init();

	/* register the driver as /dev/vga0 */
	io_link("/dev/vga0", RP_CONS(getpid(), 0));
	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
