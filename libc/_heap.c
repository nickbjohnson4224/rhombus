#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <khaos/config.h>
#include <khaos/kernel.h>

#include "libc.h"

#define HEAP_SIZE (HEAP_MXBRK - HEAP_START)

static struct _heap_allocator _ba; /* Big Allocator */
/*static struct _heap_allocator _qa; */ /* Quad Allocator */
/*static struct _heap_allocator _ca; */ /* Cell Allocator */

/* This MUST be sorted by descending maximum allocation size */
static struct _heap_allocator *_heap_alg[] = { &_ba, NULL };

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

	for (i = 0; _heap_alg[i]; i++);
	i--;

	for (; (int) i >= 0; i--) {
		if (_heap_alg[i]->cont(ptr)) {
			_heap_alg[i]->free(ptr);
			return;
		}
	}
}

size_t _heap_size(void *ptr) {
	size_t i;

	for (i = 0; _heap_alg[i]; i++);
	i--;

	for (; (int) i >= 0; i--) {
		if (_heap_alg[i]->cont(ptr)) {
			return _heap_alg[i]->size(ptr);
		}
	}

	return 0;
}

/*** Big Allocator ***/

/* The Big Allocator (_ba_*) can allocate chunks of
 * size 4096. It feeds into the tuple allocator and
 * variable allocator. It runs in constant time and 
 * can ensure no memory waste even when fragmented, 
 * because it uses the paging system directly.
 */

static uint32_t _ba_bitmap[HEAP_SIZE / (0x1000 * 8 * sizeof(uint32_t))];
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
	memclr(&_ba_bitmap[0], HEAP_SIZE / (0x1000 * 8));
}

static void *_ba_alloc(size_t size) {
	register size_t i, j;
	uint32_t word;

	if (size > 0x1000) return NULL;

	for (i = 0; _ba_bitmap[i] == (uint32_t) -1; i++) {
		if (i > HEAP_SIZE / 0x1000) return NULL;
	}
	word = _ba_bitmap[i];

	for (j = 0; j < 32; j++) {
		if ((word & (1 << j)) == 0) break;
	}

	_ba_bitmap[i] |= (1 << j);
	mmap_call(HEAP_START + (((i * 32) + j) * 0x1000), 0x1000, MMAP_RW);

	return (void*) (HEAP_START + (((i * 32) + j) * 0x1000));
}

static void _ba_free(void *ptr) {
	uintptr_t index = ((uintptr_t) ptr - HEAP_START) / 0x1000;

	if ((_ba_bitmap[index / 32] & (1 << (index % 32))) == 0) {
		return;
	}
	else {
		umap_call((uint32_t) ptr, 0x1000);
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

/*
static uintptr_t *bucket[HEAP_INIT_SIZE >> 2];

static uintptr_t heap_start = HEAP_START;
static uintptr_t heap_size = 0;
static uintptr_t *heap_fetch(size_t size);
static uintptr_t *heap_push(uintptr_t *block);
static uintptr_t *heap_pull(uintptr_t *block);
static uintptr_t *heap_split(uintptr_t *block, size_t size);
static uintptr_t *heap_merge(uintptr_t *block);
static size_t heap_get_bucket(size_t size);

void _heap_init(void) {
	uintptr_t *block;

	memset(bucket, -1, sizeof(uintptr_t*) * (HEAP_INIT_SIZE >> 2));
	heap_size = HEAP_INIT_SIZE;
	mmap_call(heap_start, heap_size * sizeof(uintptr_t), MMAP_RW);

	block = (uintptr_t*) heap_start;
	block[0] = (uintptr_t) -1;
	block[1] = heap_size;

	heap_push(block);
}

void *_heap_alloc(size_t size) {
	uintptr_t *block;

	size += sizeof(uintptr_t) - 1;
	size /= sizeof(uintptr_t);
	size += 2;

	block = heap_fetch(size);
	if (!block) return NULL;

	block = heap_split(block, size);
	block[0] = (uintptr_t) NULL;

	return &block[2];
}

void _heap_free(void *ptr) {
	uintptr_t *block = (void*) ((uintptr_t) ptr - 8);

	if (!ptr) return;

	heap_push(block);
	heap_merge(block);
}

size_t _heap_size(void *ptr) {
	return ((uintptr_t*) ptr)[1];
}

static size_t heap_get_bucket(size_t size) {
	return (size >> 2);
}

static size_t inverse_bucket(size_t size) {
	return (size << 2);
}

static uintptr_t *heap_fetch(size_t size) {
	uintptr_t *block;
	
	size = heap_get_bucket(size);
	block = bucket[size];
	if (block == (uintptr_t*) -1) {
		return heap_fetch(inverse_bucket(size+1));
	}

	bucket[size] = (uintptr_t*) bucket[size][0];
	return block;
}

static uintptr_t *heap_push(uintptr_t *block) {
	size_t size;
	
	if (!block) return NULL;

	size = heap_get_bucket(block[1]);
	block[0] = (uintptr_t) bucket[size];
	bucket[size] = block;

	return block;
}

static uintptr_t *heap_split(uintptr_t *block, size_t size) {
	size_t bsize, nsize;
	uintptr_t *nblock;

	if (!block) return NULL;

	bsize = block[1];
	block[1] = size;

	nblock = &block[size];
	nsize = bsize - size;
	nblock[1] = bsize - size;
	heap_push(nblock);

	return block;
}

static uintptr_t *heap_merge(uintptr_t *block) {
	size_t size;

	return block;
	if (!block) return NULL;

	size = block[1];

	if (block[size]) {
		heap_pull(&block[size]);
		size += block[size+1];
	}

	return block;
}

static uintptr_t *heap_pull(uintptr_t *block) {
	uintptr_t *cblock;
	size_t size;

	if (!block) return NULL;

	size = block[1];
	cblock = bucket[heap_get_bucket(size)];
	for (; cblock != (uintptr_t*) -1; cblock = (uintptr_t*) cblock[0]) {
		if (cblock[0] == (uintptr_t) block) {
			cblock[0] = block[0];
			return block;
		}
	}

	return NULL;
}*/
