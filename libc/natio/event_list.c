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

#include <stdlib.h>
#include <natio.h>

struct event_list *event_list_add(struct event_list *list, uint64_t target) {
	struct event_list *new;

	list = event_list_del(list, target);

	new = malloc(sizeof(struct event_list));
	new->target = target;
	new->next   = list;
	new->prev   = NULL;

	return new;
}

struct event_list *event_list_del(struct event_list *list, uint64_t target) {
	struct event_list *node;

	for (node = list; node; node = node->next) {
		if (node->target == target) {
			break;
		}
	}

	if (!node) {
		return list;
	}

	if (node->next) {
		node->next->prev = node->prev;
	}
	if (node->prev) {
		node->prev->next = node->next;
	}
	else {
		list = node->next;
	}

	free(node);

	return list;
}
