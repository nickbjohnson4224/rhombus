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

#ifndef __RLIBC_LAYOUT_H
#define __RLIBC_LAYOUT_H

#include <rho/arch.h>
#include <stdint.h>

/*****************************************************************************
 * System Layout Table (SLT)
 *
 * The SLT is a globally-accessible data structure with a fixed address that
 * is used to keep track of and allocate regions of the current process'
 * address space.
 */

struct slt32_entry {
	uint32_t base;
	uint32_t size;
	uint32_t type;
	uint32_t flags;

	uint32_t sub_type;
	uint32_t dyn_base;
	uint32_t reserved0;
	uint32_t next;

	uint32_t hash;
	char     name[28];
} __attribute__((packed));

// base types
#define SLT_TYPE_NULL		0
#define SLT_TYPE_KERNEL		1
#define SLT_TYPE_STACK		2
#define SLT_TYPE_OBJECT		3
#define SLT_TYPE_ALLOC		4
#define SLT_TYPE_SHARED		5
#define SLT_TYPE_RESERVED	6
#define SLT_TYPE_MISC		7

// object subtypes
#define SLT_OBJ_ELF_EXEC	1
#define SLT_OBJ_ELF_SO		2

// allocator subtypes
#define SLT_ALLOC_LIBC		1

// miscellaneous subtypes
#define SLT_MISC_ENVIRON	1
#define SLT_MISC_ARGLIST	2
#define SLT_MISC_FILETAB	3
#define SLT_MISC_CONNTAB	4

// flags
#define SLT_FLAG_CLEANUP	0x01

// constants
#define SLT_ASLR_PADDING	0x3000

struct slt32_header {
	uint32_t count;
	uint32_t mutex;
	uint32_t reserved[12];
	uint32_t first;
	uint32_t free;
	struct slt32_entry table[];
};

/* DO NOT CALL */
void sltreset(void);

void *sltalloc    (const char *name, size_t size);
void  sltfree_addr(void *base);
void  sltfree_name(const char *name);

struct slt32_entry *sltget_name(const char *name);
struct slt32_entry *sltget_addr(void *addr);

uint32_t slthash(const char *string);

#endif/*__RLIBC_LAYOUT_H*/
