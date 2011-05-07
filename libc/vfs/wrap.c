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

struct vfs_obj *(*_vfs_cons)(uint64_t source, int type);
int             (*_vfs_push)(uint64_t source, struct vfs_obj *obj);
int             (*_vfs_pull)(uint64_t source, struct vfs_obj *obj);
int             (*_vfs_free)(uint64_t source, struct vfs_obj *obj);

/* wrapper registering functions ********************************************/

int vfs_set_cons(struct vfs_obj *(*vfs_cons)(uint64_t source, int type)) {
	_vfs_cons = vfs_cons;
	return 0;
}

int vfs_set_push(int (*vfs_push)(uint64_t source, struct vfs_obj *obj)) {
	_vfs_push = vfs_push;
	return 0;
}

int vfs_set_pull(int (*vfs_pull)(uint64_t source, struct vfs_obj *obj)) {
	_vfs_pull = vfs_pull;
	return 0;
}

int vfs_set_free(int (*vfs_free)(uint64_t source, struct vfs_obj *obj)) {
	_vfs_free = vfs_free;
	return 0;
}
