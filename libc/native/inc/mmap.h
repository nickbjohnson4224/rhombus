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

#ifndef MMAP_H
#define MMAP_H

#include <arch.h>
#include <abi.h>

#define MMAP_PAGESIZE PAGESZ

#define PROT_NONE 0
#define PROT_READ MMAP_READ
#define PROT_WRITE MMAP_WRITE
#define PROT_EXEC MMAP_EXEC

int mmap(void *addr, size_t length, int prot);
int umap(void *addr, size_t length);
int emap(void *addr, uint32_t frame, int prot);
uintptr_t phys(void *addr);

#endif/*MMAP_H*/
