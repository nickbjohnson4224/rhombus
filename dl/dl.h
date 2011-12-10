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

#ifndef __DL_H
#define __DL_H

#include <stdint.h>

#include <rho/exec.h>
#include <rho/page.h>
#include <rho/arch.h>
#include <rho/elf.h>

/*****************************************************************************
 * the dynamic linker
 *
 * These functions belong to the dynamic linker, part of the C library. In
 * Rhombus, all execution is done completely from userspace: the dynamic linker
 * is a key part of this execution cycle. The C library will set up a certain
 * environment for the linker, then load it at address 0xC0000000.
 */

void *_load(void *image, size_t size, int flags);
int   _exec(void *image, size_t size, int flags);

int   _init(void *object);
int   _fini(void *object);

char *_dep  (void *object, uint32_t index, int loaded);
void *_sym  (void *object, const char *symbol);
void  _uload(void *object);

uint32_t _plt_resolve(struct elf32_ehdr *image, uint32_t index);
void    __plt_resolve(void);

/* Executable and Linking Format ********************************************/

int elf_load (const struct elf32_ehdr *image, uintptr_t base);
int elf_check(const struct elf32_ehdr *image);

/* ELF parse acceleration via caching */

struct elf_cache {
	const struct elf32_ehdr *image;
	uint32_t                 vsize;

	const struct elf32_phdr *segtab;
	const struct elf32_dyn  *dynamic;
	const struct elf32_sym  *symtab;
	const struct elf32_rel  *reltab;
	uint32_t                 reltabn;
	const struct elf32_rel  *jmprel;
	uint32_t                 jmpreln;
	const char              *strtab;
	const char              *soname;
	const uint32_t          *hash;
	const uint32_t          *pltgot;
};

void elf_gencache(struct elf_cache *cache, const struct elf32_ehdr *image, int loaded);

uint32_t elfc_resolve     (struct elf_cache *cache, const char *symbol);
uint32_t elfc_relocate    (struct elf_cache *cache, const struct elf32_rel *rel);
void     elfc_relocate_all(struct elf_cache *cache);
void     elfc_relocate_now(struct elf_cache *cache);

int dl_enter(void *entry_ptr);

#endif/*__DL_H*/
