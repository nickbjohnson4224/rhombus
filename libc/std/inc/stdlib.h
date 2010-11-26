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

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

/* memory management (internal) ********************************************/

struct __heap_node {
	struct __heap_node *next;
	struct __heap_node *prev;

	struct __heap_node *parent;
	struct __heap_node *left;
	struct __heap_node *right;

	uintptr_t base;
	uintptr_t size;
};

struct __heap_node *__new_heap_node(void);
void                __del_heap_node(struct __heap_node *node);

/* memory management *******************************************************/

void   *calloc(size_t nmemb, size_t size);
void   *valloc(size_t size);
void   *malloc(size_t size);
void   *aalloc(size_t size, size_t align);
size_t msize(void *ptr);
void   free(void *ptr);
void   *realloc(void *ptr, size_t size);

/* type conversion *********************************************************/

int  __digit(char digit, int base);

int     atoi(const char *nptr);
double  atof(const char *nptr);
int32_t atol(const char *nptr);

double      strtod (const char *nptr, char **endptr);
float       strtof (const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);

int32_t  strtol (const char *nptr, char **endptr, int base);
uint32_t strtoul(const char *nptr, char **endptr, int base);

/* random number generaton *************************************************/

#define RAND_MAX UINT32_MAX

void    srand(uint32_t seed);
uint32_t rand(void);

/* environment variables and system ****************************************/

const char *getenv (const char *name);
int         setenv (const char *name, const char *value);
void      __saveenv(void);
void      __loadenv(void);

int   system(const char *s);

/* process control *********************************************************/

#define EXIT_FAILURE	1
#define EXIT_SUCCESS	0

extern struct __atexit_func {
	struct __atexit_func *next;
	void (*function)(void);
} *__atexit_func_list;

void exit  (int status) __attribute__ ((noreturn));
void abort (void)       __attribute__ ((noreturn));
int  atexit(void (*function)(void));

/* math functions **********************************************************/

typedef struct {
	int quot;
	int rem;
} div_t;

typedef struct {
	long int quot;
	long int rem;
} ldiv_t;

int      abs(int n);
long int labs(long int n);

div_t  div(int num, int denom);
ldiv_t ldiv(long int num, long int denom);

/* searching and sorting ***************************************************/

const void *bsearch(const void *key, const void *base, size_t n, size_t size,
	int (*cmp)(const void *keyval, const void *datum));

void qsort(void *base, size_t n, size_t size, 
	int (*cmp)(const void *keyval, const void *datum));

#endif/*STDLIB_H*/
