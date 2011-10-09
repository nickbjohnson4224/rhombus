/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <string.h>
#include <stdio.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/io.h>

#include "initrd.h"
#include "inc/tar.h"

size_t initrd_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	uint8_t *file_data;
	off_t   *file_size;

	file_data = robject_data(self, "data");
	file_size = robject_data(self, "size");

	if (!file_data || !file_size) {
		return 0;
	}

	if (offset >= *file_size) {
		return 0;
	}

	if (offset + size >= *file_size) {
		size = *file_size - offset;
	}

	memcpy(buffer, &file_data[offset], size);
	
	return size;
}

void initrd_init(void) {
	struct robject *file;
	off_t *size;

	rdi_init();

	file = rdi_file_cons(0, PERM_READ);
	robject_set(0, file);
	robject_root = file;

	robject_set_data(file, "data", (void*) BOOT_IMAGE);

	size = malloc(sizeof(off_t));
	*size = tar_size((void*) BOOT_IMAGE);
	robject_set_data(file, "size", (void*) size);

	rdi_global_read_hook = initrd_read;
}
