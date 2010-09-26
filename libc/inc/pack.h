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

#ifndef PACK_H
#define PACK_H

#include <stdint.h>
#include <stdbool.h>
#include <arch.h>

#define PACK_VECT_ADDR	(ESPACE + 0x10000000)
#define PACK_HEAP_ADDR	(ESPACE + 0x10010000)
#define PACK_END_ADDR	(ESPACE + 0x20000000)

#define PACK_KEY_ENV	0x01000000
#define PACK_KEY_ARG	0x02000000
#define PACK_KEY_FILE	0x03000000

extern struct pack_list {
	struct pack_list *next;
	uint32_t key;
	void *data;
	size_t size;
} *__pack_list;

extern struct pack_vector {
	uint32_t key;
	void *data;
	size_t size;
} *__pack_vector;

extern bool __pack_usable;

void *__pack_alloc(size_t size);
void  __pack_reset(void);
void  __pack_add  (uint32_t key, const void *data, size_t size);
void  __pack_save (void);
void *__pack_load (uint32_t key, size_t *size);

#endif/*PACK_H*/
