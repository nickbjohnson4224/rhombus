/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repan
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

void update(int x1, int y1, int x2, int y2) {
	uint32_t pixel;
	int i;

	for (int x = x1; x < x2; x++) {
		for (int y = y1; y < y2; y++) {
			i = x + y * mode->width;
			pixel = (screen[i * 4] << 16) | (screen[i * 4 + 1] << 8) | screen[i * 4 + 2];
			mode->plot(x, y, pixel);
		}
	}
}

char *vga_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	char *rets = NULL;
	int x, y, width, height;

	if (!strcmp(args, "getmode")) {
		rets = strdup("320 200 32");
	}
	else if (!strcmp(args, "listmodes")) {
		rets = strdup("320:200:32");
	}
	else if (!strcmp(args, "unshare")) {
		mutex_spin(&file->mutex);
		page_free(screen, mode->width * mode->height * 4);
		free(screen);
		screen = valloc(0x20000);
		mutex_free(&file->mutex);
		rets = strdup("T");
	}
	else if (!strncmp(args, "syncrect ", 9)) {
		if (sscanf(args + 9, "%i %i %i %i", &x, &y, &width, &height) == 4) {
			update(x, y, x + width, y + height);
			rets = strdup("T");
		}
		else {
			rets = strdup("");
		}
	}

	return rets;
}

int vga_sync(uint64_t source, struct vfs_obj *file) {
	if (!screen) {
		return -1;
	}

	mutex_spin(&file->mutex);
	update(0, 0, mode->width, mode->height);
	mutex_free(&file->mutex);

	return 0;
}

int vga_share(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off) {

	if (size != mode->width * mode->height * 4) {
		return -1;
	}
	if (off != 0) {
		return -1;
	}

	mutex_spin(&file->mutex);

	if (screen) {
		page_free(screen, mode->width * mode->height * 4);
		free(screen);
		screen = NULL;
	}

	screen = buffer;

	mutex_free(&file->mutex);

	return 0;
}

size_t vga_read(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off) {
	size_t i;

	mutex_spin(&file->mutex);

	if (size > mode->width * mode->height * 4 - off) {
		size = mode->width * mode->height * 4 - off;
	}

	for (i = 0; i < size; i++) {
		buffer[i] = screen[i + off];
	}

	mutex_free(&file->mutex);

	return size;
}

size_t vga_write(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off) {
	size_t i;

	mutex_spin(&file->mutex);

	if (size > mode->width * mode->height * 4 - off) {
		size = mode->width * mode->height * 4 - off;
	}

	for (i = 0; i < size; i++) {
		screen[i + off] = buffer[i];
	}

	mutex_free(&file->mutex);

	return size;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;

	root        = calloc(sizeof(struct vfs_obj), 1);
	root->type  = RP_TYPE_FILE;
	root->size  = 0;
	root->acl   = acl_set_default(root->acl, PERM_READ | PERM_WRITE);
	vfs_set_index(0, root);

	vmem   = valloc(0x20000);
	screen = valloc(0x20000);
	page_phys(vmem, 0x20000, PROT_READ | PROT_WRITE, 0xA0000);

	vga_set_mode(MODE_320x200x256);

	mode->fill(0, 0, mode->width, mode->height, 0);

	/* set up driver interface */
	di_wrap_sync (vga_sync);
	di_wrap_share(vga_share);
	di_wrap_rcall(vga_rcall);
	di_wrap_read (vga_read);
	di_wrap_write(vga_write);
	vfs_wrap_init();

	/* register the driver as /dev/vga0 */
	io_link("/dev/vga0", RP_CONS(getpid(), 0));
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
