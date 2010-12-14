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

#ifndef DL_H
#define DL_H

#include <stdint.h>

/*****************************************************************************
 * the dynamic linker
 *
 * These functions belong to the dynamic linker, part of the C library. In
 * Flux, all execution is done completely from userspace: the dynamic linker
 * is a key part of this execution cycle. The C library will set up a certain
 * environment for the linker, then copy the contents of the ELF section ".dl"
 * as well as any necessary files into a special region of memory. This 
 * section acts as an independent program which then loads the executable.
 *
 * Because of this mechanism, the rest of the C library cannot be called from
 * the linker once it has been moved, so don't try to do that! The dynamic
 * linker has its own versions of basic string functions and system call 
 * stubs.
 */

/* libc interface functions *************************************************/

struct _dl_list {
	int  type;
	void *ptr;
	char name[56];
};

int _dl_exec(struct _dl_list *list);

/* dynamic linker ***********************************************************/

int      _dl_entry(struct _dl_list *list);

/* dynamic linker string functions ******************************************/

void     *_dl_memcpy(void *dst, const void *src, size_t size);
void     *_dl_memclr(void *ptr, size_t size);
char     *_dl_strcpy(char *dst, const char *src);
int       _dl_strcmp(const char *s1, const char *s2);
size_t    _dl_strlen(const char *str);

/* dynamic linker memory management *****************************************/

extern uintptr_t _dl_load_brk;
extern uintptr_t _dl_temp_brk;

void     *_dl_alloc_temp(size_t size);
void     *_dl_alloc_load(size_t size);

/* dynamic linker syscalls **************************************************/

void      _dl_page(uintptr_t address, size_t count, uint32_t perm, int source, uintptr_t offset);
void      _dl_exit(uint32_t value) __attribute__ ((noreturn));

#endif/*DL_H*/
