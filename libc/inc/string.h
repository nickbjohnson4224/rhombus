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

#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

/* memory functions ********************************************************/

void  *memchr (const void *, uint8_t, size_t n);
int	   memcmp (const void *, const void *, size_t);

void  *memcpy (void *, const void *, size_t);
void  *memmove(void *, const void *, size_t);

void  *memset (void *, uint8_t, size_t);
void  *memclr (void *, size_t);

/* basic string functions **************************************************/

char  *strcpy (char *, const char *);
char  *strncpy(char *, const char *, size_t);
size_t strlcpy(char *, const char *, size_t);

char  *strcat (char *, const char *);
char  *strncat(char *, const char *, size_t);
size_t strlcat(char *, const char *, size_t);
char  *strvcat(const char *, ...);

int    strcmp (const char *, const char *);
int    strncmp(const char *, const char *, size_t);

size_t strlen (const char *);

char  *strdup  (const char *);
char  *struntil(const char *, const char *, const char **);
char  *strwhile(const char *, const char *, const char **);

/* advanced string functions ***********************************************/

char  *strchr (const char *, char);
char  *strrchr(const char *, char);

char  *strstr (const char *, const char *);
char  *strpbrk(const char *, const char *);

size_t strspn (const char *, const char *);
size_t strcspn(const char *, const char *);

char  *strtok  (char *, const char *);
char  *strtok_r(char *, const char *, char **);

char **strparse(const char *, const char *);

const char *strerror(int);

/* string locales ***********************************************************/
int    strcoll(const char *, const char *);
size_t strxfrm(char *, const char *, size_t);

#endif
