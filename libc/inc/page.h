/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef __RLIBC_PAGE_H
#define __RLIBC_PAGE_H

#include <arch.h>
#include <abi.h>

#define PROT_NONE 	0
#define PROT_READ 	1
#define PROT_WRITE 	2
#define PROT_EXEC 	4
#define PROT_LOCK	8
#define PROT_LINK	16

#define PAGE_NULL	0
#define PAGE_ANON	1
#define PAGE_PACK	2
#define PAGE_PHYS	3
#define PAGE_SELF	4
#define PAGE_PROT	5

int page(void *addr, size_t length, int prot, int source, uintptr_t off);

int page_free(void *addr, size_t length);
int page_anon(void *addr, size_t length, int prot);
int page_pack(void *addr, size_t length, int prot);
int page_phys(void *addr, size_t length, int prot, uintptr_t base);
int page_self(void *addrs, void *addrd, size_t length);
int page_prot(void *addr, size_t length, int prot);

uintptr_t phys(void *addr);

#endif/*__RLIBC_PAGE_H*/
