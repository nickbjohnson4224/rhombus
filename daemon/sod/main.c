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

#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <natio.h>
#include <stdio.h>
#include <proc.h>
#include <page.h>

static void  sod_init(int argc, char **argv);
static void *sod_mmap(struct fs_obj *file, size_t size, uint64_t offset, int prot);

struct driver sod_driver = {
	sod_init,

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	sod_mmap,

	NULL,
};

/*****************************************************************************
 * sod - shared object daemon
 *
 * Usage: sod <path>
 *
 * Creates a shared object daemon instance that pulls from the directory 
 * <path>. This creates a cache of that directory that will give out read-only 
 * shared memory containing the directory's files' contents. This is mostly 
 * used to implement shared libraries efficiently.
 *
 * If <path> is not given, <path> is assumed to be "/lib".
 */

static void sod_init(int argc, char **argv) {
	struct fs_obj *file;
	uint64_t src;
	uint64_t dir;
	uint32_t inode = 1;
	char *name, *path;
	size_t i;

	path = (argc > 1) ? argv[1] : "/lib";

	dir = fs_find(0, path);

	if (!dir) {
		fprintf(stderr, "%s: no such directory\n", (argc > 1) ? argv[1] : "/lib");
		abort();
	}

	/* add root directory */
	file = calloc(sizeof(struct fs_obj), 1);
	file->type  = FOBJ_DIR;
	file->inode = 0;
	file->acl   = acl_set_default(file->acl, FS_PERM_READ);
	lfs_root(file);

	/* add directory contents */
	for (i = 0;; i++) {
		name = fs_list(dir, i);

		if (name) {
			if (strstr(name, ".so")) {
				src  = fs_find(0, strvcat(path, "/", name, NULL));
				file = calloc(sizeof(struct fs_obj), 1);
				file->type  = FOBJ_FILE;
				file->inode = inode++;
				file->acl   = acl_set_default(file->acl, FS_PERM_READ);
				lfs_add(file, name);

				file->size = fs_size(src);
				if (file->size % PAGESZ) {
					file->size = file->size - (file->size % PAGESZ) + PAGESZ;
				}

				file->data = aalloc(file->size, PAGESZ);
				if (!file->data) continue;
			
				read(src, file->data, fs_size(src), 0);
			}
		}
		else {
			break;
		}
	}
}

static void *sod_mmap(struct fs_obj *file, size_t size, uint64_t offset, int prot) {
	
	if (!file->data) {
		return NULL;
	}

	if (offset > file->size) {
		return NULL;
	}

	if (offset % PAGESZ) {
		return NULL;
	}

	if (offset + size > file->size) {
		return NULL;
	}

	if (prot & PROT_WRITE) {
		return NULL;
	}
	
	return &file->data[offset];
}

int main(int argc, char **argv) {

	driver_init(&sod_driver, argc, argv);

	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
