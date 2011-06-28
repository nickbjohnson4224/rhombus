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

#include <rdi/vfs.h>
#include <rdi/access.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

static char *_size_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_type_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_getperm_handler(uint64_t source, uint32_t index, int argc, char **argv);
static char *_setperm_handler(uint64_t source, uint32_t index, int argc, char **argv);

/*****************************************************************************
 * vfs_init
 *
 * Initialize VFS request handling system. Returns zero on success, nonzero on
 * error.
 */

int vfs_init(void) {

	rcall_set("fs_type", _type_handler);
	rcall_set("fs_size", _size_handler);
	rcall_set("fs_getperm", _getperm_handler);
	rcall_set("fs_setperm", _setperm_handler);

	return 0;
}

static char *_size_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint64_t size;

	r = index_get(index);
	if (!r) return strdup("! nfound");

	mutex_spin(&r->mutex);
	if (!vfs_permit(r, source, PERM_READ)) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	if ((r->type & FS_TYPE_FILE) == 0 || (r->type & FS_TYPE_CHAR) != 0) {
		mutex_free(&r->mutex);
		return strdup("! type");
	}

	size = r->size;
	mutex_free(&r->mutex);

	return saprintf("%d:%d", (uint32_t) (size >> 32), (uint32_t) size);
}

static char *_type_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	char *reply;

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	reply = malloc(sizeof(char) * 2);
	reply[1] = '\0';
	reply[0] = typechar(r->type);

	return reply;
}

static char *_getperm_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint32_t user;
	uint8_t perm;

	if (argc < 2) {
		return NULL;
	}

	user = atoi(argv[1]);
	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);
	perm = id_hash_get(&r->acl, user);
	mutex_free(&r->mutex);

	return saprintf("%d", perm);
}

static char *_setperm_handler(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint32_t user;
	uint8_t perm;

	if (argc < 3) {
		return NULL;
	}

	user = atoi(argv[1]);
	perm = atoi(argv[2]);

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);

	if (!vfs_permit(r, source, PERM_ALTER)) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	id_hash_set(&r->acl, user, perm);

	mutex_free(&r->mutex);

	return strdup("T");
}
