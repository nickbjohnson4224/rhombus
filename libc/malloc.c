#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <khaos/config.h>
#include <khaos/kernel.h>

#define HEAP_INIT_SIZE 0x1000

static uintptr_t *bucket[1024];

static uintptr_t heap_start = HEAP_START;
static uintptr_t heap_size = 0;
static uintptr_t *heap_fetch(size_t size);
static uintptr_t *heap_push(uintptr_t *block);
static uintptr_t *heap_pull(uintptr_t *block);
static uintptr_t *heap_split(uintptr_t *block, size_t size);
static uintptr_t *heap_merge(uintptr_t *block);
static size_t heap_get_bucket(size_t size);

void init_heap(void) {
	uintptr_t *block;

	memset(bucket, -1, sizeof(uintptr_t*) * 1024);
	heap_size = HEAP_INIT_SIZE;
	mmap_call(heap_start, heap_size, MMAP_RW);
	bucket[heap_get_bucket(heap_size)] = (uintptr_t*) heap_start;

	block = (uintptr_t*) heap_start;
	block[0] = (uintptr_t) -1;
	block[1] = heap_size;

	heap_push(block);
}

void *calloc(size_t nmemb, size_t size) {
	void *ptr = malloc(nmemb * size);
	memset(ptr, 0, (nmemb * size));
	return ptr;
}

void *malloc(size_t size) {
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

void free(void *ptr) {
	uintptr_t *block = ptr;

	if (!block) return;
	if (heap_pull(block)) return;

	heap_push(block);
	heap_merge(block);
}

void *realloc(void *ptr, size_t size) {
	void *nptr = malloc(size);
	memcpy(nptr, ptr, ((uintptr_t*) ptr)[1] * sizeof(uintptr_t));
	free(ptr);
	return nptr;
}

static size_t heap_get_bucket(size_t size) {
	register size_t r = size, v;

	return (size >> 2);

	r --;
	r |= r >> 1;
	r |= r >> 2;
	r |= r >> 4;
	r |= r >> 8;
	r |= r >> 16;
	r ++;

	v = r;
	r = (size & (v >> 1)) ? 0x1 : 0x0;
	r |= ((v & 0xAAAAAAAA) != 0) << 1;
	r |= ((v & 0xCCCCCCCC) != 0) << 2;
	r |= ((v & 0xF0F0F0F0) != 0) << 3;
	r |= ((v & 0xFF00FF00) != 0) << 4;
	r |= ((v & 0xFFFF0000) != 0) << 5;

	return r;
}

static size_t inverse_bucket(size_t size) {
	register size_t r;

	return (size << 2);

	r = 1 << (size >> 1);
	if (size & 0x1) r += (r >> 1);

	return r;
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
