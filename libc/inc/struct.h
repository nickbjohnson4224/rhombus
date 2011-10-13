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

#ifndef __RLIBC_STRUCT_H
#define __RLIBC_STRUCT_H

#include <stdint.h>

/*****************************************************************************
 * General Purpose Lookup Table
 */

struct s_table_entry {
   struct s_table_entry *next;
   uint32_t hash;
   char *string;
   void *data;
};

struct s_table {
   size_t size; // real size is (1 << size)
   size_t load;
   struct s_table_entry table[];
};

struct s_table *s_table_set (struct s_table *table, const char *field, void *data);
void           *s_table_get (struct s_table *table, const char *field);

struct s_table *s_table_seti(struct s_table *table, uint32_t field, void *data);
void           *s_table_geti(struct s_table *table, uint32_t field);

struct s_table *s_table_setv(struct s_table *table, void *data, const char *field, ...);
void           *s_table_getv(struct s_table *table, const char *field, ...);

void            s_table_free(struct s_table *table);
void            s_table_iter(struct s_table *table, void (*iter)(const char*, void*));

#endif/*__RLIBC_STRUCT_H*/
