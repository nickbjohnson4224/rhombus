/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <space.h>

struct heap_block {
	struct heap_block *next;
};

/****************************************************************************
 * heap_bucket
 * 
 * Array of singly linked lists of free blocks in the allocator. The size
 * of blocks in bucket n is 2^n pointer-sized words (4 bytes on x86).
 */

static struct heap_block *heap_bucket[10];

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
	void *block;

	/* find appropriately sized bucket */
	bucket = -1;
	for (i = 0; i < 10; i++) {
		if (size < (sizeof(uintptr_t) * (1 << i))) {
			bucket = i;
			break;
		}
	}

	if (bucket == -1) {
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

	block = heap_bucket[bucket];
	heap_bucket[bucket] = heap_bucket[bucket]->next;

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
	bucket = -1;
	for (i = 0; i < 10; i++) {
		if (size <= (sizeof(uintptr_t) * (1 << i))) {
			bucket = i;
			break;
		}
	}

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

void heap_new_slab(size_t bucket) {
	uintptr_t *slab;
	uintptr_t i;

	slab = heap_valloc();

	if (!slab) {
		/* out of memory */
		return;
	}

	/* dump slab into bucket */
	for (i = 0; i < PAGESZ / sizeof(uintptr_t); i += (1 << bucket)) {
		heap_free(&slab[i], sizeof(uintptr_t) * (1 << bucket));
	}
}

/****************************************************************************
 * heap_valloc
 *
 * Returns a pointer to a single page of kernel memory. This memory is
 * accessible from all address spaces, privileged, and read-write.
 * Returns null on out of memory error. Virtual memory taken from this
 * allocator cannot be freed.
 */

void *heap_valloc(void) {
	static uintptr_t brk = KERNEL_HEAP;

	if (brk + PAGESZ >= KERNEL_HEAP_END) {
		/* out of memory */
		return NULL;
	}
	else {
		page_set(brk, page_fmt(frame_new(), PF_PRES | PF_RW));

		brk += PAGESZ;
		return (void*) (brk - PAGESZ);
	}
}
