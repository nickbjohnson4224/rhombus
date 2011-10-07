/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef _RDI_IO_H
#define _RDI_IO_H

#include <rdi/core.h>
#include <ipc.h>

/*****************************************************************************
 * rdi_class_file (extends rdi_class_core) - file
 *
 * Calls:
 *
 * size - R
 *
 * reset - W
 *
 * Fields:
 *
 * size
 *
 *   Type: off_t * (heap-allocated)
 *
 * read
 *
 *   Type: rdi_read_hook (function pointer)
 *
 * write
 *
 *   Type: rdi_write_hook (function pointer)
 *
 * mmap
 *
 *   Type: rdi_mmap_hook (function pointer)
 */

extern struct robject *rdi_class_file;
void __rdi_class_file_setup();

struct robject *rdi_file_cons(uint32_t index, uint32_t access);
void            rdi_file_free(struct robject *r);

typedef size_t (*rdi_read_hook) (struct robject *r, rp_t src, uint8_t *buf, size_t size, off_t off);
typedef size_t (*rdi_write_hook)(struct robject *r, rp_t src, uint8_t *buf, size_t size, off_t off);
typedef void * (*rdi_mmap_hook) (struct robject *r, rp_t src, size_t size, off_t off, int prot);

extern rdi_read_hook  rdi_global_read_hook;
extern rdi_write_hook rdi_global_write_hook;
extern rdi_mmap_hook  rdi_global_mmap_hook;

/*
 * Old stuff past here
 */

/* I/O callbacks ************************************************************/

void rdi_set_read (size_t (*_read) (uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_write(size_t (*_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_reset(void   (*_reset)(uint64_t src, uint32_t idx));
void rdi_set_sync (void   (*_sync) (uint64_t src, uint32_t idx));
void rdi_set_share(int    (*_share)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_mmap (void  *(*_mmap) (uint64_t src, uint32_t idx, size_t size, uint64_t off, int prot));

/* RDI I/O handlers *********************************************************/

void rdi_init_io();

void __rdi_read_handler (struct msg *msg);
void __rdi_write_handler(struct msg *msg);
void __rdi_reset_handler(struct msg *msg);
void __rdi_sync_handler (struct msg *msg);
void __rdi_share_handler(struct msg *msg);
void __rdi_mmap_handler (struct msg *msg);

#endif/*_RDI_IO_H*/
