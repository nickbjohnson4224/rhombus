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

#include <string.h>
#include <stdlib.h>
#include <driver.h>

#include "tmpfs.h"

uint8_t tmpfs_inode_top = 1;

void tmpfs_init(int argc, char **argv) {
	struct fs_obj *root;

	root = calloc(sizeof(struct fs_obj), 1);
	root->type = FOBJ_DIR;
	root->inode = 0;
	root->acl = acl_set_default(root->acl, ACL_READ | ACL_WRITE);

	lfs_root(root);
}

struct fs_obj *tmpfs_cons(int type) {
	struct fs_obj *fobj = NULL;
	
	switch (type) {
	case FOBJ_FILE:
		fobj        = calloc(sizeof(struct fs_obj), 1);
		fobj->type  = FOBJ_FILE;
		fobj->size  = 0;
		fobj->data  = NULL;
		fobj->inode = tmpfs_inode_top++;
		fobj->acl   = acl_set_default(fobj->acl, ACL_READ | ACL_WRITE);
		break;
	case FOBJ_DIR:
		fobj        = calloc(sizeof(struct fs_obj), 1);
		fobj->type  = FOBJ_DIR;
		fobj->inode = tmpfs_inode_top++;
		fobj->acl   = acl_set_default(fobj->acl, ACL_READ | ACL_WRITE);
		fobj->link  = 0;
		break;
	}

	return fobj;
}

int tmpfs_free(struct fs_obj *obj) {

	acl_free(obj->acl);

	if (obj->data) {
		free(obj->data);
	}

	free(obj);

	return 0;
}

size_t tmpfs_read(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	
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

size_t tmpfs_write(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	
	if (offset + size >= file->size) {
		file->data = realloc(file->data, offset + size);
		file->size = offset + size;
	}

	memcpy(&file->data[offset], buffer, size);

	return size;
}

int tmpfs_reset(struct fs_obj *file) {
	
	if (file->data) {
		free(file->data);
		file->size = 0;
		file->data = NULL;
	}

	return 0;
}

struct driver tmpfs_driver = {
	tmpfs_init, 

	tmpfs_cons,
	NULL,
	NULL,
	tmpfs_free,

	NULL,
	tmpfs_read,
	tmpfs_write,
	tmpfs_reset,
	NULL,
	NULL,

	NULL,
};
