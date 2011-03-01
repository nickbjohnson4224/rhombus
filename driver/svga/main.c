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
#include <stdio.h>
#include <mutex.h>
#include <page.h>
#include <proc.h>
#include <ipc.h>

#include "svga.h"

uint32_t *buffer;
char *modesstr;

char *svga_rcall(uint64_t source, struct vfs_obj *file, const char *args) {
	char *rets = NULL;
	int x, y, d, w, h;
	int mode;

	if (!strcmp(args, "getmode")) {
		rets = malloc(16);
		sprintf(rets, "%d %d %d", svga.w, svga.h, svga.d);
		return rets;
	}
	if (!strcmp(args, "listmodes")) {
		return strdup(modesstr);
	}
	if (!strcmp(args, "unshare")) {
		mutex_spin(&file->mutex);
		page_free(buffer, msize(buffer));
		free(buffer);
		buffer = valloc(svga.w * svga.h * 4);
		mutex_free(&file->mutex);
		return strdup("T");
	}

	if (!strncmp(args, "setmode ", 8)) {
		if (sscanf(args + 8, "%i %i %i", &x, &y, &d) != 3) {
			return strdup("");
		}
		mutex_spin(&file->mutex);
		mode = svga_find_mode(x, y, d);
		if (svga_set_mode(mode)) {
			return strdup("");
		}
		page_free(buffer, msize(buffer));
		free(buffer);
		buffer = valloc(svga.w * svga.h * 4);
		mutex_free(&file->mutex);
		return strdup("T");
	}

	if (!strncmp(args, "syncrect ", 9)) {
		if (sscanf(args + 9, "%i %i %i %i", &x, &y, &w, &h) != 4) {
			return strdup("");
		}
		mutex_spin(&file->mutex);
		svga_fliprect(buffer, x, y, w, h);
		mutex_free(&file->mutex);
		return strdup("T");
	}
	
	return NULL;
}

int svga_sync(uint64_t source, struct vfs_obj *file) {
	
	if (!buffer) {
		return -1;
	}

	mutex_spin(&file->mutex);
	svga_flip(buffer);
	mutex_free(&file->mutex);

	return 0;
}

int svga_share(uint64_t source, struct vfs_obj *file, uint8_t *_buffer, size_t size, uint64_t off) {
	
	if (size != svga.w * svga.h * 4) {
		return -1;
	}
	if (off != 0) {
		return -1;
	}

	mutex_spin(&file->mutex);

	if (buffer) {
		page_free(buffer, msize(buffer));
		free(buffer);
	}

	buffer = (uint32_t*) _buffer;

	mutex_free(&file->mutex);

	return 0;
}

size_t svga_read(uint64_t source, struct vfs_obj *file, uint8_t *_buffer, size_t size, uint64_t off) {
	size_t i;

	mutex_spin(&file->mutex);

	if (size > svga.w * svga.h * 4 - off) {
		size = svga.w * svga.h * 4 - off;
	}

	for (i = 0; i < size; i++) {
		_buffer[i] = ((uint8_t*) buffer)[i + off];
	}

	mutex_free(&file->mutex);

	return size;
}

size_t svga_write(uint64_t source, struct vfs_obj *file, uint8_t *_buffer, size_t size, uint64_t off) {
	size_t i;

	mutex_spin(&file->mutex);

	if (size > svga.w * svga.h * 4 - off) {
		size = svga.w * svga.h * 4 - off;
	}

	for (i = 0; i < size; i++) {
		((uint8_t*) buffer)[i + off] = buffer[i];
	}

	mutex_free(&file->mutex);

	return size;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;
	char *modesstr0;
	char *modestr;
	int i;

	root       = calloc(sizeof(struct vfs_obj), 1);
	root->type = RP_TYPE_FILE;
	root->size = 0;
	root->acl  = acl_set_default(root->acl, PERM_READ | PERM_WRITE);

	svga_init();

	// generate list of modes
	modesstr = strdup("");
	for (i = 0; i < modelist_count; i++) {
		modesstr0 = modesstr;
		modestr = malloc(16);
		sprintf(modestr, "%d:%d:%d ", modelist[i].w, modelist[i].h, modelist[i].d);
		modesstr = strvcat(modesstr, modestr, NULL);
		free(modesstr0);
	}

	svga_set_mode(svga_find_mode(640, 480, 24));
	buffer = calloc(svga.w * svga.h * 4, 1);
	svga_flip(buffer);

	/* set up driver interface */
	di_wrap_sync (svga_sync);
	di_wrap_share(svga_share);
	di_wrap_rcall(svga_rcall);
	di_wrap_read (svga_read);
	di_wrap_write(svga_write);
	vfs_wrap_init();

	/* register the driver as /dev/svga0 */
	io_link("/dev/svga0", RP_CONS(getpid(), 0));
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
