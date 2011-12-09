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

void *_sym  (void *object, const char *symbol);
void  _uload(void *object);
int   _error(void);

/* Executable and Linking Format ********************************************/

int elf_load (const struct elf32_ehdr *image, uintptr_t base);
int elf_check(const struct elf32_ehdr *image);

const struct elf32_phdr *elf_get_segtab  (const struct elf32_ehdr *image);
uintptr_t                elf_get_vsize   (const struct elf32_ehdr *image);
const struct elf32_dyn  *elf_get_dynamic (const struct elf32_ehdr *image);
const struct elf32_sym  *elf_get_symtab  (const struct elf32_ehdr *image);
const char              *elf_get_strtab  (const struct elf32_ehdr *image);
const char              *elf_get_soname  (const struct elf32_ehdr *image);
const struct elf32_sym  *elf_get_symbol  (const struct elf32_ehdr *image, const char *symbol);
const struct elf32_rel  *elf_get_reltab  (const struct elf32_ehdr *image, size_t *count);

void                     elf_relocate_all(struct elf32_ehdr *image);

uint32_t elf_resolve_local(const struct elf32_ehdr *image, const char *symbol);
uint32_t elf_resolve      (const struct elf32_ehdr *image, const char *symbol);

int dl_enter(void *entry_ptr);

#endif/*__DL_H*/
