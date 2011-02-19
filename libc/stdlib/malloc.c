/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex.h>
#include <errno.h>
#include <page.h>
#include <arch.h>

static struct __heap_node *_tree;
static struct __heap_node *_list[32];
static bool _mutex 		= false;

static void                _add_to_list  (struct __heap_node *node);
static struct __heap_node *_get_by_addr  (uintptr_t addr);
static struct __heap_node *_find_node    (uintptr_t index);
static uintptr_t            ilog2        (uintptr_t n);

/*****************************************************************************
 * aalloc
 *
 * Allocates a block of size <size> and alignment a multiple of <align> from
 * the heap and returns a pointer to it. If the alignment is not a power of
 * two, the request will be rejected. If the alignment is zero, the alignment
 * will be decided by the allocator. Returns null on failure.
 */

void *aalloc(size_t size, size_t align) {

	if (align) {
		if (align != ((size_t) 1 << ilog2(align))) {
			errno = EINVAL;
			return NULL;
		}

		size = (size > align) ? size : align;
	}

	return malloc(size);
}

/*****************************************************************************
 * malloc
 *
 * Allocates a block of size <size> from the heap and returns a pointer to it.
 * The block is readable and writable. Returns null on failure.
 */

void *malloc(size_t size) {
	struct __heap_node *node;
	uintptr_t index;

	index = ilog2(size);

	mutex_spin(&_mutex);
	node = _find_node(index);
	mutex_free(&_mutex);

	if (!node) {
		errno = ENOMEM;
		return NULL;
	}
	else {
		if (page_anon((void*) node->base, 1 << index, PROT_READ | PROT_WRITE)) {
			/* could not allocate memory */
			errno = ENOMEM;
			return NULL;
		}

		return (void*) node->base;
	}
}

size_t msize(void *ptr) {
	struct __heap_node *node;
	uintptr_t base = (uintptr_t) ptr;
	size_t size;

	mutex_spin(&_mutex);
	node = _get_by_addr(base);
	size = (node) ? ((size_t) 1 << node->size) : 0;
	mutex_free(&_mutex);

	return size;
}

void free(void *ptr) {
	struct __heap_node *node;
	uintptr_t base = (uintptr_t) ptr;

	mutex_spin(&_mutex);
	node = _get_by_addr(base);

	if (node) {
		_add_to_list(node);

//		if (node->size >= 12) {
//			page_free((void*) node->base, (size_t) 1 << node->size);
//		}
	}
//	else {
//		printf("invalid free of %x at %x\n", ptr, ((int*) &ptr)[2]);
//		abort();
//	}

	mutex_free(&_mutex);
}

/****************************************************************************
 * ilog2
 *
 * Returns the ceiling of the base 2 logarithm of the given integer. Uses a 
 * unrolled version of the algorithm detailed at 
 * <http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog>.
 */

static uintptr_t ilog2(uintptr_t n) {
	register uintptr_t r = 0;
	uintptr_t orig_n = n;

	if (n &   0xFFFF0000) {
		n >>= 0x10;
		r |=  0x10;
	}
	if (n &   0xFF00) {
		n >>= 0x08;
		r |=  0x08;
	}
	if (n &   0xF0) {
		n >>= 0x04;
		r |=  0x04;
	}
	if (n &   0xC) {
		n >>= 0x02;
		r |=  0x02;
	}
	if (n &   0x2) {
		n >>= 0x01;
		r |=  0x01;
	}

	if (((uintptr_t) 1 << r) == orig_n) {
		return r;
	}
	else {
		return r + 1;
	}
}

/*****************************************************************************
 * _find_node
 *
 * Returns a block of the size index <index>, allocated from the heap. If 
 * there is no adequate block in the free list, a larger block is split in
 * two: the lower half is returned and the higher half added to the free list.
 * Returns null on out of memory error.
 */

static struct __heap_node *_find_node(uintptr_t index) {
	struct __heap_node *node;

	if (index >= 32) {
		return NULL;
	}

	if (!_tree) {
		_tree = __new_heap_node();
		_tree->base = HEAP_START;
		_tree->size = ilog2(HEAP_MXBRK - HEAP_START);

		_add_to_list(_tree);
	}

	node = _list[index];
	if (node) {
		_list[index] = node->next;
		return node;
	}
	else {
		node = _find_node(index + 1);
		if (!node) {
			return NULL;
		}
		else {
			node->left = __new_heap_node();
			node->left->left   = NULL;
			node->left->right  = NULL;
			node->left->parent = node;
			node->left->base   = node->base;
			node->left->size   = index;

			node->right = __new_heap_node();
			node->right->left   = NULL;
			node->right->right  = NULL;
			node->right->parent = node;
			node->right->base   = node->base + (1 << index);
			node->right->size   = index;
			
			_add_to_list(node->right);

			return node->left;
		}
	}
}

/*****************************************************************************
 * _add_to_list
 *
 * Adds the block <node> to the proper free list. This function is not
 * thread-safe.
 */

static void _add_to_list(struct __heap_node *node) {
	uintptr_t index = node->size;

	node->next = _list[index];
	_list[index] = node;
}

/*****************************************************************************
 * _get_by_addr
 *
 * Searches the heap for an allocated block with base address <addr>. Returns
 * the found block on success, null on failure. This function is not 
 * thread-safe.
 */

static struct __heap_node *_get_by_addr(uintptr_t addr) {
	struct __heap_node *root;

	root = _tree;

	while (root) {
		if (root->left && root->right) {
			root = (addr < root->right->base) ? root->left : root->right;
		}
		else {
			return (addr == root->base) ? root : NULL;
		}
	}

	return NULL;
}
