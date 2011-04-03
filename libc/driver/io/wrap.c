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

size_t   (*_di_read) (uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
size_t   (*_di_write)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
int      (*_di_reset)(uint64_t source, struct vfs_obj *file);
int      (*_di_sync) (uint64_t source, struct vfs_obj *file);
uint64_t (*_di_size) (uint64_t source, struct vfs_obj *file);
int      (*_di_share)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
char *   (*_di_rcall)(uint64_t source, struct vfs_obj *file, const char *args);

/* wrapper registering functions ********************************************/

int di_wrap_read(size_t (*di_read)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off)) {
	when(PORT_READ, __read_wrapper);
	_di_read = di_read;
	return 0;
}

int di_wrap_write(size_t (*di_write)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off)) {
	when(PORT_WRITE, __write_wrapper);
	_di_write = di_write;
	return 0;
}

int di_wrap_reset(int (*di_reset)(uint64_t source, struct vfs_obj *file)) {
	when(PORT_RESET, __reset_wrapper);
	_di_reset = di_reset;
	return 0;
}

int di_wrap_sync(int (*di_sync)(uint64_t source, struct vfs_obj *file)) {
	when(PORT_SYNC, __sync_wrapper);
	_di_sync = di_sync;
	return 0;
}

int di_wrap_size(uint64_t (*di_size)(uint64_t source, struct vfs_obj *file)) {
	_di_size = di_size;
	return 0;
}

int di_wrap_share(int (*di_share)(uint64_t source, struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off)) {
	when(PORT_SHARE, __share_wrapper);
	_di_share = di_share;
	return 0;
}

int di_wrap_rcall(char* (*di_rcall)(uint64_t source, struct vfs_obj *file, const char *args)) {
	when(PORT_RCALL, __rcall_wrapper);
	_di_rcall = di_rcall;
	return 0;
}
