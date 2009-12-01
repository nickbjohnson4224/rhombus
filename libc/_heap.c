#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <khaos/config.h>
#include <khaos/kernel.h>

#include "libc.h"

#define HEAP_SIZE (HEAP_MXBRK - HEAP_START)

static struct _heap_allocator _ba; /* Big Allocator */
static struct _heap_allocator _qa; /* Quad Allocator */
static struct _heap_allocator _ca; /* Cell Allocator */

/* This MUST be sorted by ascending maximum allocation size */
static struct _heap_allocator *_heap_alg[] = { &_ca, &_qa, &_ba, NULL };

/*** The Heap ***/

/* This file contains the entire Khaos C standard
 * library heap. It is divided into three allocators
 * which ensure efficiency in different situations.
 * Very large allocations are handled by the Big
 * Allocator (_ba_*), 2- and 4-dword allocations are
 * handled by the Cell and Quad Allocators, 
 * respectively, (_ca_*, _qa_*), and all other 
 * allocations are handled by the Variable Allocator 
 * (_va_*).
 */

void _heap_init(void) {
	size_t i;

	for (i = 0; _heap_alg[i]; i++) {
		_heap_alg[i]->init();
	}
}

void *_heap_alloc(size_t size) {
	size_t i;

	for (i = 0; _heap_alg[i]; i++) {
		if (_heap_alg[i]->max_size >= size) {
			return _heap_alg[i]->alloc(size);
		}
	}

	return NULL;
}

void _heap_free(void *ptr) {
	size_t i;

	for (i = 0; _heap_alg[i]; i++) {
		if (_heap_alg[i]->cont(ptr)) {
			_heap_alg[i]->free(ptr);
			return;
		}
	}
}

size_t _heap_size(void *ptr) {
	size_t i;

	for (i = 0; _heap_alg[i]; i++) {
		if (_heap_alg[i]->cont(ptr)) {
			return _heap_alg[i]->size(ptr);
		}
	}

	return 0;
}

/*** Big Allocator ***/

/* The Big Allocator (_ba_*) can allocate single
 * pages. It feeds into the tuple allocator and
 * variable allocator. It does not run fast, but 
 * can ensure no memory waste even when fragmented, 
 * because it uses the kernel memory ABI directly.
 */

static uint32_t _ba_bitmap[HEAP_SIZE / (PAGESZ * 8 * sizeof(uint32_t))];
static void _ba_init(void);
static void *_ba_alloc(size_t);
static void _ba_free(void*);
static bool _ba_cont(void*);
static size_t _ba_size(void*);

static struct _heap_allocator _ba = {
	0x1000,
	&_ba_init,
	&_ba_alloc,
	&_ba_free,
	&_ba_cont,
	&_ba_size,
};

static void _ba_init(void){
	memclr(&_ba_bitmap[0], HEAP_SIZE / (PAGESZ * 8));
}

static void *_ba_alloc(size_t size) {
	register size_t i, j;
	uint32_t word;

	if (size > PAGESZ) return NULL;

	for (i = 0; _ba_bitmap[i] == (uint32_t) -1; i++) {
		if (i > HEAP_SIZE / PAGESZ) return NULL;
	}
	word = _ba_bitmap[i];

	for (j = 0; j < 32; j++) {
		if ((word & (1 << j)) == 0) break;
	}

	_ba_bitmap[i] |= (1 << j);
	mmap_call(HEAP_START + (((i * 32) + j) * PAGESZ), PAGESZ, MMAP_RW);

	return (void*) (HEAP_START + (((i * 32) + j) * PAGESZ));
}

static void _ba_free(void *ptr) {
	uintptr_t index = ((uintptr_t) ptr - HEAP_START) / PAGESZ;

	if ((_ba_bitmap[index / 32] & (1 << (index % 32))) == 0) {
		return;
	}
	else {
		umap_call((uint32_t) ptr, PAGESZ);
		_ba_bitmap[index / 32] &= ~(1 << (index % 32));
	}
}

static bool _ba_cont(void *ptr) {
	uintptr_t addr = (uintptr_t) ptr;

	if (addr >= HEAP_START && addr < HEAP_MXBRK) {
		return true;
	}
	
	return false;
}

static size_t _ba_size(void *ptr) {
	ptr = NULL;
	return 0x1000;
}

/*** Cell Allocator ***/

/* The Cell Allocator (_ca_*) can allocate chunks of
 * size 8, or two pointers on this architecture. It
 * is used internally by other allocators, but also
 * is an efficient way of allocating small chunks of
 * memory, for things like trees and linked lists.
 */

struct _ca_cell {
	struct _ca_cell *car;
	struct _ca_cell *cdr;
};

static struct _ca_cell *_ca_block_list;
static struct _ca_cell *_ca_free_list;
static void _ca_init(void);
static void *_ca_alloc(size_t);
static void _ca_free(void*);
static bool _ca_cont(void*);
static size_t _ca_size(void*);

static struct _ca_cell *_ca_new_block(void);
static bool _ca_find(void *ptr, struct _ca_cell *block);

static struct _heap_allocator _ca = {
	2 * sizeof(void*),
	&_ca_init,
	&_ca_alloc,
	&_ca_free,
	&_ca_cont,
	&_ca_size,
};

static void _ca_init(void) {
	_ca_block_list = NULL;
	_ca_free_list = _ca_new_block();
}

static void *_ca_alloc(size_t size) {
	struct _ca_cell *t;

	if (!_ca_free_list) return NULL;

	t = _ca_free_list;
	_ca_free_list = _ca_free_list->cdr;

	return t;
}

static void _ca_free(void *ptr) {
	struct _ca_cell *cell = ptr;

	cell->cdr = _ca_free_list;
	_ca_free_list = cell;
}

static bool _ca_cont(void *ptr) {
	struct _ca_cell *block = _ca_block_list;

	while (block) {
		if (_ca_find(ptr, block) == true) {
			return true;
		}
		block = block->cdr;
	}

	return false;
}

static size_t _ca_size(void *ptr) {
	ptr = NULL;
	return (2 * sizeof(void*));
}

static struct _ca_cell *_ca_new_block(void) {
	struct _ca_cell *block = malloc(PAGESZ);
	size_t i;

	block->cdr = _ca_block_list;
	_ca_block_list = block;

	for (i = 1; i < PAGESZ / sizeof(struct _ca_cell) - 1; i++) {
		block[i].cdr = &block[i+1];
	}
	block[i].cdr = NULL;

	return &block[1];
}

static bool _ca_find(void *ptr, struct _ca_cell *block) {
	uintptr_t addr = (uintptr_t) ptr;
	uintptr_t base = (uintptr_t) &block[1];

	if (addr >= base && addr < base + PAGESZ) return true;
	return false;
}

/*** Quad Allocator ***/

/* The Quad Allocator (_qa_*) is a copy of the 
 * Cell Allocator that allocates chunks of size
 * 16 instead of 8, or four pointers on this
 * architecture. It is more suitable for use in
 * trees and more complex data structures.
 */

struct _qa_cell {
	struct _qa_cell *car;
	struct _qa_cell *cdr;
	uintptr_t extra[2];
};

static struct _qa_cell *_qa_block_list;
static struct _qa_cell *_qa_free_list;
static void _qa_init(void);
static void *_qa_alloc(size_t);
static void _qa_free(void*);
static bool _qa_cont(void*);
static size_t _qa_size(void*);

static struct _qa_cell *_qa_new_block(void);
static bool _qa_find(void *ptr, struct _qa_cell *block);

static struct _heap_allocator _qa = {
	4 * sizeof(void*),
	&_qa_init,
	&_qa_alloc,
	&_qa_free,
	&_qa_cont,
	&_qa_size,
};

static void _qa_init(void) {
	_qa_block_list = NULL;
	_qa_free_list = _qa_new_block();
}

static void *_qa_alloc(size_t size) {
	struct _qa_cell *t;

	if (!_qa_free_list) return NULL;

	t = _qa_free_list;
	_qa_free_list = _qa_free_list->cdr;

	return t;
}

static void _qa_free(void *ptr) {
	struct _qa_cell *cell = ptr;

	cell->cdr = _qa_free_list;
	_qa_free_list = cell;
}

static bool _qa_cont(void *ptr) {
	struct _qa_cell *block = _qa_block_list;

	while (block) {
		if (_qa_find(ptr, block) == true) {
			return true;
		}
		block = block->cdr;
	}

	return false;
}

static size_t _qa_size(void *ptr) {
	ptr = NULL;
	return (4 * sizeof(void*));
}

static struct _qa_cell *_qa_new_block(void) {
	struct _qa_cell *block = malloc(PAGESZ);
	size_t i;

	block->cdr = _qa_block_list;
	_qa_block_list = block;

	for (i = 1; i < PAGESZ / sizeof(struct _qa_cell) - 1; i++) {
		block[i].cdr = &block[i+1];
	}
	block[i].cdr = NULL;

	return &block[1];
}

static bool _qa_find(void *ptr, struct _qa_cell *block) {
	uintptr_t addr = (uintptr_t) ptr;
	uintptr_t base = (uintptr_t) &block[1];

	if (addr >= base && addr < base + PAGESZ) return true;
	return false;
}

