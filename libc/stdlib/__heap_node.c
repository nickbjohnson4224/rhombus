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
#include <mutex.h>
#include <page.h>
#include <arch.h>

static uintptr_t _brk 				= HEAP2_START;
static struct __heap_node *_stack 	= NULL;
static bool _mutex 					= false;

struct __heap_node *__new_heap_node(void) {
	struct __heap_node *node;

	mutex_spin(&_mutex); {
		if (_stack) {
			node   = _stack;
			_stack = node->next;
		}
		else {
			node = (void*) _brk;
			_brk += sizeof(struct __heap_node);

			page_anon(node, sizeof(struct __heap_node), PROT_READ | PROT_WRITE);
		}
	} mutex_free(&_mutex);

	memclr(node, sizeof(struct __heap_node));
	return node;
}

void __del_heap_node(struct __heap_node *node) {

	mutex_spin(&_mutex); {
		node->next = _stack;
		_stack = node;
	} mutex_free(&_mutex);
}
