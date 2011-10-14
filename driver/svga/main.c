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
#include <stdio.h>
#include <mutex.h>
#include <page.h>
#include <proc.h>
#include <ipc.h>

#include <rdi/core.h>
#include <rdi/arch.h>
#include <rdi/vfs.h>
#include <rdi/io.h>

#include "svga.h"

uint32_t *buffer;
char *modesstr;

char *svga_rcall_getmode(struct robject *self, rp_t source, int argc, char **argv) {
	return saprintf("%d %d %d", svga.w, svga.h, svga.d);
}

char *svga_rcall_listmodes(struct robject *self, rp_t source, int argc, char **argv) {
	return strdup(modesstr);
}

char *svga_rcall_unshare(struct robject *self, rp_t source, int argc, char **argv) {

	mutex_spin(&self->driver_mutex);
	page_free(buffer, msize(buffer));
	free(buffer);
	buffer = valloc(svga.w * svga.h * 4);
	mutex_free(&self->driver_mutex);

	return strdup("T");
}

char *svga_rcall_setmode(struct robject *self, rp_t source, int argc, char **argv) {
	int x, y, d;
	int mode;

	if (argc != 4) return NULL;

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	d = atoi(argv[3]);

	mutex_spin(&self->driver_mutex);
	mode = svga_find_mode(x, y, d);
	if (svga_set_mode(mode)) return NULL;
	page_free(buffer, msize(buffer));
	free(buffer);
	buffer = valloc(svga.w * svga.h * 4);
	mutex_free(&self->driver_mutex);

	return strdup("T");
}

char *svga_rcall_syncrect(struct robject *self, rp_t source, int argc, char **argv) {
	int x, y, w, h;

	if (argc != 5) return NULL;

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	w = atoi(argv[3]);
	h = atoi(argv[4]);

	mutex_spin(&self->driver_mutex);
	svga_fliprect(buffer, x, y, w, h);
	mutex_free(&self->driver_mutex);

	return strdup("T");
}

char *svga_rcall_sync(struct robject *self, rp_t source, int argc, char **argv) {
	svga_flip(buffer);
	return strdup("T");
}

int svga_share(struct robject *self, rp_t source, uint8_t *_buffer, size_t size, uint64_t off) {

	if (size != svga.w * svga.h * 4) {
		return -1;
	}
	if (off != 0) {
		return -1;
	}

	if (buffer) {
		page_free(buffer, msize(buffer));
		free(buffer);
	}

	buffer = (uint32_t*) _buffer;

	return 0;
}

int main(int argc, char **argv) {
	struct robject *canvas;
	char *modesstr0;
	char *modestr;
	int i;

	rdi_init();

	canvas = rdi_file_cons(robject_new_index(), ACCS_READ | ACCS_WRITE);
	robject_set_data(canvas, "type", (void*) "canvas share");

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
	buffer = malloc(svga.w * svga.h * 4);

	/* set up driver interface */
	robject_set_call(canvas, "getmode",   svga_rcall_getmode,   STAT_READER);
	robject_set_call(canvas, "listmodes", svga_rcall_listmodes, STAT_READER);
	robject_set_call(canvas, "unshare",   svga_rcall_unshare,   STAT_WRITER);
	robject_set_call(canvas, "setmode",   svga_rcall_setmode,   STAT_WRITER);
	robject_set_call(canvas, "syncrect",  svga_rcall_syncrect,  STAT_WRITER);
	robject_set_call(canvas, "sync",      svga_rcall_sync,      STAT_WRITER);
	rdi_global_share_hook = svga_share;

	/* register the driver as /dev/svga0 */
	fs_plink("/dev/svga0", RP_CONS(getpid(), canvas->index), NULL);
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
