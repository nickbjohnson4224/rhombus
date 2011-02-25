/*
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

#include <wmanager.h>
#include <natio.h>
#include <page.h>
#include <proc.h>
#include <string.h>

uint64_t wm_create_window(size_t width, size_t height) {
	static int next_name = 0;
	char name[16];
	uint64_t fd;

	sprintf(name, "/sys/wmanager/%i-%i", getpid(), next_name++);
	fd = io_cons(name, RP_TYPE_FILE);
	if (!fd) {
		return 0;
	}
	if (wm_set_size(fd, width, height) != 0) {
		wm_destroy_window(fd);
		return 0;
	}
	return fd;	
}

int wm_set_size(uint64_t window, size_t width, size_t height) {
	char buf[16];
	sprintf(buf, "s %i %i", width, height);
	return rcall(window, buf) ? 0 : -1;
}

int	wm_set_bitmap(uint64_t window, uint8_t *bitmap, size_t size) {
	return share(window, bitmap, size, 0, PROT_READ);
}

int wm_update(uint64_t window) {
	return sync(window);
}

int wm_destroy_window(uint64_t window) {
	return fs_remove(window);
}
