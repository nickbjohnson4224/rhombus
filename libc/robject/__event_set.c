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

#include <robject.h>

struct __robject_event_set *__event_set_add(struct __robject_event_set *set, rp_t target) {
	struct __robject_event_set *new_node;

	new_node = malloc(sizeof(struct __robject_event_set));
	new_node->next = set;
	new_node->prev = NULL;

	if (set) {
		set->prev = new_node;
	}

	return new_node;
}

struct __robject_event_set *__event_set_del(struct __robject_event_set *set, rp_t target) {
	struct __robject_event_set *curr;

	for (curr = set; curr; curr = curr->next) {
		if (curr->target == target) {
			if (curr->prev) {
				curr->prev->next = curr->next;
			}
			if (curr->next) {
				curr->next->prev = curr->prev;
			}

			if (!curr->prev) {
				set = curr->next;
			}

			free(curr);
			break;
		}
	}

	return set;
}

void __event_set_send(struct __robject_event_set *set, const char *value) {
	
	while (set) {
		event(set->target, value);
		set = set->next;
	}
}

void __event_set_free(struct __robject_event_set *set) {
	struct __robject_event_set *temp;

	while (set) {
		temp = set->next;
		free(set);
		set = temp;
	}
}
