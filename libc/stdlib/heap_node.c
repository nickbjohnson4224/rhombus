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

#include "malloc.h"

#include <rho/mutex.h>
#include <rho/page.h>

static uintptr_t brk = HEAP2_START;
static struct heap_node *stack;
static bool heap_node_mutex;

struct heap_node *new_heap_node(void) {
	struct heap_node *node;

	mutex_spin(&heap_node_mutex);
	if (stack) {
		node  = stack;
		stack = node->next;
	}
	else {
		node = (void*) brk;
		brk += sizeof(struct heap_node);

		page_anon(node, sizeof(struct heap_node), PROT_READ | PROT_WRITE);
	}
	mutex_free(&heap_node_mutex);

	return node;
}

void del_heap_node(struct heap_node *node) {
	mutex_spin(&heap_node_mutex);
	node->next = stack;
	stack = node;
	mutex_free(&heap_node_mutex);
}
