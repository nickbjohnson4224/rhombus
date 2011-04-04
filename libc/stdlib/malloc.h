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

#ifndef STDLIB_MALLOC_H
#define STDLIB_MALLOC_H

#include <stdlib.h>
#include <arch.h>

/*****************************************************************************
 * dynamic memory allocator layout
 * 
 * The heap is divided into two sections: one for allocating heap nodes, and
 * one for general use. The former section is from HEAP2_START to HEAP2_END,
 * and the latter is from HEAP_START to HEAP_END.
 */

#define HEAP_START 	0x10000000
#define HEAP_END	0x30000000

#define HEAP2_START HEAP_END
#define HEAP2_END	(HEAP_END + 0x1000000)

/*****************************************************************************
 * internal allocator
 *
 * Allocates only elements of the type struct heap_node.
 */

struct heap_node {
	struct heap_node *next;

	struct heap_node *parent;
	struct heap_node *left;
	struct heap_node *right;

	uintptr_t status; // 1 if allocated, 0 if free
	uintptr_t base;   // base address
	uintptr_t size;   // base 2 logarithm of size in bytes
	uintptr_t padding;
};

struct heap_node *new_heap_node(void);
void del_heap_node(struct heap_node *node);

#endif/*STDLIB_MALLOC_H*/
