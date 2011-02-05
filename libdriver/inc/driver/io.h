/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef DRIVER_IO_H
#define DRIVER_IO_H

#include <stdint.h>
#include <natio.h>
#include <ipc.h>

#include <driver/vfs.h>

/* I/O request wrapper ******************************************************/

int di_wrap_read (size_t   (*di_read) (struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off));
int di_wrap_write(size_t   (*di_write)(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off));
int di_wrap_reset(int      (*di_reset)(struct vfs_obj *file));
int di_wrap_sync (int      (*di_sync) (struct vfs_obj *file));
int di_wrap_size (uint64_t (*di_size) (struct vfs_obj *file));
int di_wrap_share(int      (*di_share)(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off));

void __read_wrapper (struct msg *msg);
void __write_wrapper(struct msg *msg);
void __sync_wrapper (struct msg *msg);
void __reset_wrapper(struct msg *msg);
void __share_wrapper(struct msg *msg);

extern size_t   (*_di_read) (struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
extern size_t   (*_di_write)(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);
extern int      (*_di_reset)(struct vfs_obj *file);
extern int      (*_di_sync) (struct vfs_obj *file);
extern uint64_t (*_di_size) (struct vfs_obj *file);
extern int      (*_di_share)(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t off);

#endif/*DRIVER_IO_H*/
