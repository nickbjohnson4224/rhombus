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

#include <driver.h>
#include <stdint.h>
#include <ipc.h>

/* wrapper function pointers ************************************************/

struct resource *(*_vfs_cons) (uint64_t source, int type);
int              (*_vfs_sync) (uint64_t source, struct resource *r);
int              (*_vfs_free) (uint64_t source, struct resource *r);
int              (*_vfs_open) (uint64_t source, struct resource *r);
int              (*_vfs_close)(uint64_t source, struct resource *r);

/* wrapper registering functions ********************************************/

int vfs_set_cons(struct resource *(*vfs_cons)(uint64_t source, int type)) {
	_vfs_cons = vfs_cons;
	return 0;
}

int vfs_set_sync(int (*vfs_sync)(uint64_t source, struct resource *obj)) {
	_vfs_sync = vfs_sync;
	return 0;
}

int vfs_set_free(int (*vfs_free)(uint64_t source, struct resource *obj)) {
	_vfs_free = vfs_free;
	return 0;
}

int vfs_set_open(int (*vfs_open) (uint64_t source, struct resource *obj)) {
	_vfs_open = vfs_open;
	return 0;
}

int vfs_set_close(int (*vfs_close)(uint64_t source, struct resource *obj)) {
	_vfs_close = vfs_close;
	return 0;
}
