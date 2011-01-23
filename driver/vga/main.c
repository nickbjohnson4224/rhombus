/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repan
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
#include <mutex.h>

#include "vga.h"

uint8_t *vmem;
uint8_t *screen;

size_t vga_read(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	char data[16];

	sprintf(data, "%i %i", mode->width, mode->height);

	size_t length = strlen(data) - offset;
	if (length > size) {
		length = size;
	}

	memcpy(buffer, data + offset, length);
	return length;
}

int vga_sync(struct vfs_obj *file) {
	if (!screen) {
		return -1;
	}
	mutex_spin(&file->mutex);
	for (size_t i = 0; i < mode->width * mode->height; i++) {
		uint32_t pixel = (screen[i * 3] << 16) | (screen[i * 3 + 1] << 8) | screen[i * 3 + 2];
		mode->plot(i % mode->width, i / mode->width, pixel);
	}
	mutex_free(&file->mutex);
	return 0;
}

int vga_mmap(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off) {
	if (size != mode->width * mode->height * 3) {
		return -1;
	}
	if (off != 0) {
		return -1;
	}
	screen = buffer;
	return 0;
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
	di_wrap_sync(vga_sync);
	di_wrap_mmap(vga_mmap);
	vfs_wrap_init();

	/* register the driver as /dev/vga0 */
	io_link("/dev/vga0", RP_CONS(getpid(), 0));
	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
