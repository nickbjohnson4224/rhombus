#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <khaos/config.h>
#include <khaos/kernel.h>

#define HEAP_INIT_SIZE 0x1000

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
}
