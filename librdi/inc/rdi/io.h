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

/* I/O callbacks ************************************************************/

void rdi_set_read (size_t (*_read) (uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_write(size_t (*_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_reset(void   (*_reset)(uint64_t src, uint32_t idx));
void rdi_set_sync (void   (*_sync) (uint64_t src, uint32_t idx));
void rdi_set_share(void   (*_share)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off));
void rdi_set_mmap (void  *(*_mmap) (uint64_t src, uint32_t idx, size_t size, uint64_t off, int prot));

extern size_t (*_rdi_callback_read) (uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
extern size_t (*_rdi_callback_write)(uint64_t src, uint32_t idx, uint8_t *buf, size_t size, uint64_t off);
extern void   (*_rdi_callback_reset)(uint64_t src, uint32_t idx);
extern void   (*_rdi_callback_sync) (uint64_t src, uint32_t idx);
extern void   (*_rdi_callback_share)(uint64_t src, uint32_t idx);
extern void   (*_rdi_callback_mmap) (uint64_t src, uint32_t idx, size_t size, uint64_t off, int prot);

/* RDI I/O handlers *********************************************************/

void rdi_init_io();

void __rdi_read_handler (struct msg *msg);
void __rdi_write_handler(struct msg *msg);
void __rdi_reset_handler(struct msg *msg);
void __rdi_sync_handler (struct msg *msg);
void __rdi_share_handler(struct msg *msg);
void __rdi_mmap_handler (struct msg *msg);

#endif/*_RDI_IO_H*/
