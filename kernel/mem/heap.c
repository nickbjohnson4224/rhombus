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
#include <space.h>
#include <debug.h>

static void  heap_new_slab(size_t bucket);
static void *heap_valloc  (size_t size);

struct heap_block {
	struct heap_block *next;
};

/****************************************************************************
 * heap_bucket
 * 
 * Array of singly linked lists of free blocks in the allocator. The size
 * of blocks in bucket n is 2^n pointer-sized words (4 bytes on x86).
 */

static struct heap_block *heap_bucket[16];

static int analysis[16];

/****************************************************************************
 * heap_alloc
 *
 * Returns a pointer to a block of kernel memory of size size bytes. This
 * memory is accessible from all address spaces, is privileged and
 * read-write. Returns null on out of memory or too large error. The
 * block of memory is 16 byte aligned for allocations of size 9 bytes
 * or greater, and page aligned for allocations of 2049 to 4096 bytes.
 */

void *heap_alloc(size_t size) {
	size_t bucket;
	uintptr_t i;
	struct heap_block *block;

	/* find appropriately sized bucket */
	bucket = 17;
	for (i = 0; i < 16; i++) {
		if (size <= (sizeof(uintptr_t) * (1 << i))) {
			bucket = i;
			break;
		}
	}

	if (bucket > 16) {
		/* allocation was too large */
		return NULL;
	}

	if (!heap_bucket[bucket]) {
		heap_new_slab(bucket);

		if (!heap_bucket[bucket]) {
			/* out of memory */
			return NULL;
		}
	}
	
	analysis[bucket]++;

	block = heap_bucket[bucket];
	heap_bucket[bucket] = block->next;

	return block;
}

/****************************************************************************
 * heap_free
 *
 * Frees the given block of memory. The size given must be the same as the
 * size used when allocating the block, or terrible things can and likely 
 * will happen.
 */

void heap_free(void *ptr, size_t size) {
	size_t bucket;
	uintptr_t i;
	struct heap_block *block;

	/* find appropriate bucket */
	bucket = 17;
	for (i = 0; i < 16; i++) {
		if (size <= (sizeof(uintptr_t) * (1 << i))) {
			bucket = i;
			break;
		}
	}

	if (bucket > 17) {
		return;
	}

	analysis[bucket]--;

	block = ptr;

	block->next = heap_bucket[bucket];
	heap_bucket[bucket] = block;
}

/****************************************************************************
 * heap_new_slab
 *
 * Allocates and initializes a new slab of memory, and adds its contents to
 * the specified bucket in pieces of appropriate size for that bucket.
 */

static void heap_new_slab(size_t bucket) {
	uintptr_t *slab;
	uintptr_t i;
	uintptr_t slabsize;

	slabsize = (bucket <= 10) ? PAGESZ : (4 << bucket) * sizeof(uintptr_t);

	slab = heap_valloc(slabsize);

	if (!slab) {
		/* out of memory */
		return;
	}

	/* dump slab into bucket */
	for (i = 0; i < slabsize / sizeof(uintptr_t); i += (1 << bucket)) {
		heap_free(&slab[i], sizeof(uintptr_t) * (1 << bucket));
		analysis[bucket]++;
	}
}

/****************************************************************************
 * heap_valloc
 *
 * Returns a pointer to some pages of kernel memory. This memory is
 * accessible from all address spaces, privileged, and read-write.
 * Returns null on out of memory error. Virtual memory taken from this
 * allocator cannot be freed.
 */

static void *heap_valloc(uintptr_t size) {
	static uintptr_t brk = KERNEL_HEAP;
	int i;

	if (brk + size >= KERNEL_HEAP_END) {
		/* out of memory */

		/* print analysis */
		for (i = 0; i < 16; i++) {
			debug_printf("%d: %d\n", i, analysis[i]);
		}

		debug_panic("out of virtual memory");
		return NULL;
	}
	else {
		page_set(brk, page_fmt(frame_new(), PF_PRES | PF_RW));
		mem_alloc(brk, size, PF_PRES | PF_RW);

		brk += size;
		return (void*) (brk - size);
	}
}
