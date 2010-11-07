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

#ifndef TEST_H
#define TEST_H

/* libc tests **************************************************************/

int test_libc(void);

/* string tests ************************************************************/

int test_string(void);

int test_memchr(void);
int test_memcmp(void);
int test_memcpy(void);
int test_memmove(void);
int test_memset(void);
int test_memclr(void);
int test_strcpy(void);
int test_strncpy(void);
int test_strcat(void);
int test_strncat(void);
int test_strcmp(void);
int test_strncmp(void);
int test_strlen(void);
int test_strchr(void);
int test_strrchr(void);
int test_strstr(void);
int test_strpbrk(void);
int test_strspn(void);
int test_strcspn(void);
int test_strtok(void);

/* stdlib tests ************************************************************/

int test_stdlib(void);

int test_heap(void);

int test_helper(void);
int test_calloc(void);
int test_valloc(void);
int test_malloc(void);
int test_aalloc(void);
int test_msize(void);
int test_realloc(void);
int test_heap_torture(void);

int test_convert(void);

int test_atoi(void);
int test_atof(void);
int test_atol(void);
int test_strtod(void);
int test_strtof(void);
int test_strtold(void);
int test_strtol(void);
int test_strtoul(void);

int test_rand(void);

int test_environ(void);

int test_system(void);

int test_stdlib_math(void);

int test_abs(void);
int test_labs(void);
int test_div(void);
int test_ldiv(void);

int test_bsearch(void);
int test_qsort(void);

#endif/*TEST_H*/
