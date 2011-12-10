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

#ifndef __RLIBC_DLFCN_H
#define __RLIBC_DLFCN_H

struct dl {
	void *(*load) (void *image, size_t size, int flags);
	int   (*exec) (void *image, size_t size, int flags);

	int   (*init) (void *object);
	int   (*fini) (void *object);

	char *(*dep)  (void *object, uint32_t index, int loaded);
	void *(*sym)  (void *object, const char *symbol);
    void  (*uload)(void *object);

	void *(*slt_alloc)    (const char *name, size_t size);
	void  (*slt_free_addr)(void *addr);
	void  (*slt_free_name)(const char *name);
	struct slt32_entry *(*slt_get_addr)(void *addr);
	struct slt32_entry *(*slt_get_name)(const char *name);
};

extern struct dl *dl;

#define RLTD_LAZY      0x0000
#define RLTD_NOW       0x0001

#define RLTD_GLOBAL    0x0000
#define RLTD_LOCAL     0x0002
#define RLTD_IMAGE     0x0004
#define RLTD_OVERWRITE 0x0008
#define RLTD_NOLOAD    0x0010

void *dlopen (const char *filename, int flags);
void  dlclose(void *object);
void *dlsym  (void *object, const char *symbol);
char *dlerror(void);

#endif/*__RLIBC_DLFCN_H*/
