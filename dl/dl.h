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

void *_sym  (void *object);
void  _uload(void *object);
int   _error(void);

/* ELF loading **************************************************************/

int dl_elf_load (struct elf32_ehdr *file, uintptr_t base);
int dl_elf_check(struct elf32_ehdr *file);

int dl_enter(void *entry_ptr);

int dl_plt_resolve(uint32_t *got, uint32_t index);

#endif/*__DL_H*/
