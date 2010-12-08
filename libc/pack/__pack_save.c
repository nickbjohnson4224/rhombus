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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pack.h>
#include <page.h>

struct pack_vector *__pack_vector = (void*) PACK_VECT_ADDR;

void __pack_save(void) {
	size_t i;
	struct pack_list *list;

	for (list = __pack_list, i = 0; list; list = list->next, i++);

	page_anon(__pack_vector, (i + 1) * sizeof(struct pack_vector), 
		PROT_READ | PROT_WRITE);

	for (list = __pack_list, i = 0; list; list = list->next, i++) {
		__pack_vector[i].key  = list->key;
		__pack_vector[i].size = list->size;
		__pack_vector[i].data = __pack_alloc(list->size);
		memcpy(__pack_vector[i].data, list->data, list->size);
		free(list->data);
	}

	__pack_vector[i].key = 0;
}
