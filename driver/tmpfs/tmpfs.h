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

#ifndef TMPFS_H
#define TMPFS_H

#include <stddef.h>
#include <natio.h>
#include <stdio.h>
#include <mutex.h>
#include <ipc.h>

struct tmpfs_inode {
	uint32_t inode;
	char  *data;
	size_t size;
	bool  mutex;
};

void tmpfs_new  (struct vfs_query *query, uint32_t inode, uint32_t caller);
void tmpfs_del  (struct vfs_query *query, uint32_t inode, uint32_t caller);
void tmpfs_mov  (struct vfs_query *query, uint32_t inode, uint32_t caller);
void tmpfs_read (struct packet *packet, uint8_t port, uint32_t caller);
void tmpfs_write(struct packet *packet, uint8_t port, uint32_t caller);

void tmpfs_init(void);

#endif/*TMPFS_H*/
