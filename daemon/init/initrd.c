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

size_t initrd_read(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	struct resource *file;

	file = index_get(index);

	if (!file->data) {
		return 0;
	}

	if (offset >= file->size) {
		return 0;
	}

	if (offset + size >= file->size) {
		size = file->size - offset;
	}

	memcpy(buffer, &file->data[offset], size);
	
	return size;
}

void initrd_init(void) {
	struct resource *root;

	root = resource_cons(FS_TYPE_FILE, PERM_READ);
	root->data = (void*) BOOT_IMAGE;
	root->size = tar_size(root->data);
	index_set(0, root);

	rdi_set_read(initrd_read);
	rdi_init_all();
}
