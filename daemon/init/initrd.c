/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <mutex.h>
#include <ipc.h>
#include <proc.h>

#include <stdio.h>
#include <natio.h>
#include <string.h>
#include <stdlib.h>
#include <driver.h>

#include "initrd.h"
#include "inc/tar.h"

void initrd_init(int argc, char **argv) {
	struct fs_obj *root;

	root = calloc(sizeof(struct fs_obj), 1);
	root->type = FOBJ_FILE;
	root->data = (void*) BOOT_IMAGE;
	root->size = tar_size(root->data);
	root->inode = 0;

	lfs_root(root);
}

size_t initrd_read(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {

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

struct driver initrd_driver = {
	initrd_init, 

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	initrd_read,
	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
};
