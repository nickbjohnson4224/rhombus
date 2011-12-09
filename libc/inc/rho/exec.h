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

#ifndef __RLIBC_EXEC_H
#define __RLIBC_EXEC_H

#include <stdint.h>
#include <stddef.h>

/* executable loading *******************************************************/

void *load_exec  (const char *name);
void *load_shared(const char *soname);

/* execute ******************************************************************/

/* with executable image */
int execive(uint8_t *image, size_t size, char const **argv, char const **envp);
int execiv (uint8_t *image, size_t size, char const **argv);
int execi  (uint8_t *image, size_t size);

/* from filesystem */
int execve (const char *path, char const **argv, char const **envp);
int execv  (const char *path, char const **argv);
int exec   (const char *path);

/* things to save on exec ***************************************************/

char  *packarg(const char **argv);
char **loadarg(char *pack);

/* dynamic linker interface *************************************************/

struct dl {
	void *(*load) (void *image, size_t size, int flags);
	int   (*exec) (void *image, size_t size, int flags);

	int   (*init) (void *object);
	int   (*fini) (void *object);

	char *(*dep)  (void *object, uint32_t index, int loaded);
	void *(*sym)  (void *object, const char *symbol);
    void  (*uload)(void *object);
	int   (*error)(void);

	void *(*slt_alloc)    (const char *name, size_t size);
	void  (*slt_free_addr)(void *addr);
	void  (*slt_free_name)(const char *name);
	struct slt32_entry *(*slt_get_addr)(void *addr);
	struct slt32_entry *(*slt_get_name)(const char *name);
};

extern struct dl *dl;

void       *dlopen (const char *filename, int flags);
void       *dlload (void *image, size_t size, int flags);
void        dlinit (void *object);
void        dlfini (void *object);
void        dlexec (void *object, char const **argv, char const **envp);
void        dlclose(void *object);
const char *dldep  (void *object, uint32_t index, int loaded);
void       *dlsym  (void *object, const char *symbol);
char       *dlerror(void);

#endif/*__RLIBC_EXEC_H*/
