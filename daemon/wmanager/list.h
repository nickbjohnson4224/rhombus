/*
 * Copyright (C) 2011 Jaagup Repan
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

#ifndef LIST_H
#define LIST_H

#define LIST_ADD(item) \
{ \
	struct item##_t **ptr = &item##s; \
	while (*ptr) { \
		item->prev = *ptr; \
		ptr = &(*ptr)->next; \
	} \
	*ptr = item; \
}

#define LIST_REMOVE(item) \
{ \
	if (item->prev) { \
		item->prev->next = item->next; \
	} \
	else { \
		item##s = item->next; \
	} \
	if (item->next) { \
		item->next->prev = item->prev; \
	} \
	free(item); \
}

#define LIST_FIND(list, ret, cmp) \
{ \
	ret = NULL; \
	struct list##_t *item = list##s; \
	while (item) { \
		if (cmp) { \
			ret = item; \
		} \
		item = item->next; \
	} \
}

#define LIST_FREE(list) \
{ \
	struct list##_t *item = list##s; \
	while (item) { \
		struct list##_t *next = item->next; \
		free(item); \
		item = next; \
	} \
}

#endif
