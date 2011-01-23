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

#ifndef DL_H
#define DL_H

#include <stdint.h>
#include <exec.h>
#include <page.h>
#include <arch.h>
#include <elf.h>

/*****************************************************************************
 * the dynamic linker
 *
 * These functions belong to the dynamic linker, part of the C library. In
 * Flux, all execution is done completely from userspace: the dynamic linker
 * is a key part of this execution cycle. The C library will set up a certain
 * environment for the linker, then load it at address 0xC0000000.
 *
 * Currently, the dynamic linker (despite its name) can only handle statically
 * linked executables. It will ignore any shared libraries given, and choke on
 * any relocations.
 */

/* dynamic linker string functions ******************************************/

void  *dl_memcpy(void *dst, const void *src, size_t size);
void  *dl_memclr(void *ptr, size_t size);
char  *dl_strcpy(char *dst, const char *src);
int    dl_strcmp(const char *s1, const char *s2);
size_t dl_strlen(const char *str);

/* dynamic linker memory management *****************************************/

int dl_page(void *addr, size_t length, int prot, int source, uintptr_t off);

int dl_page_free(void *addr, size_t length);
int dl_page_anon(void *addr, size_t length, int prot);
int dl_page_self(void *addrs, void *addrd, size_t length);
int dl_page_prot(void *addr, size_t length, int prot);

/* dynamic linker syscalls **************************************************/

int  _dl_page(void *addr, size_t count, uint32_t perm, int source, uintptr_t offset);
void _dl_when(uintptr_t entry);

/* ELF loading **************************************************************/

int dl_elf_load (struct elf32_ehdr *file);
int dl_elf_check(struct elf32_ehdr *file);

int dl_enter(void *entry_ptr);

#endif/*DL_H*/
