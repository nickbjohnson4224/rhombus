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

#ifndef _RDI_UTIL_H
#define _RDI_UTIL_H

#include <stdint.h>
#include <stdbool.h>

/* general-purpose PID/UID to integer hashtable *****************************/

struct id_hash_list {
	struct id_hash_list *next;
	struct id_hash_list *prev;
	uint32_t id;
	uint32_t value;
};

struct id_hash {
	bool mutex;
	struct id_hash_list **table;
	int size;
	int count;
	uint32_t nil;
};

void     id_hash_set  (struct id_hash *h, uint32_t id, uint32_t value);
uint32_t id_hash_get  (struct id_hash *h, uint32_t id);
int      id_hash_test (struct id_hash *h, uint32_t id);
int      id_hash_count(struct id_hash *h);
void     id_hash_free (struct id_hash *h);

#endif/*_RDI_UTIL_H*/
