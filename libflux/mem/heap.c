/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/heap.h>
#include <flux/mmap.h>

/****************************************************************************
 * block
 *
 * Represents a single block in the heap.
 */

struct block {
	size_t size;
	struct block *next;
};

/****************************************************************************
 * brk
 *
 * Top of heap memory.
 */

static void *brk = (void*) HEAP_START;
static uint32_t m_brk;

/****************************************************************************
 * bucket
 *
 * Lists of blocks of size (1 << index).
 */

static struct block *bucket[PTRBITS];
static uint32_t m_bucket[PTRBITS];

/****************************************************************************
 * get_bucket
 *
 * Returns an index corresponding to the appropriate bucket for the given
 * block size. Uses a portable, unrolled version of the algorithm detailed
 * at <http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog>.
 */

static size_t get_bucket(size_t size) {
	register size_t rslt = 0;

	#if   (INTBITS == 64)
	if (size & 0xFFFFFFFF00000000) {
		size >>= 0x20;
		rslt |=  0x20;
	}
	#endif
	if (size & 0xFFFF0000) {
		size >>= 0x10;
		rslt |=  0x10;
	}
	if (size & 0xFF00) {
		size >>= 0x08;
		rslt |=  0x08;
	}
	if (size & 0xF0) {
		size >>= 0x04;
		rslt |=  0x04;
	}
	if (size & 0xC) {
		size >>= 0x02;
		rslt |=  0x02;
	}
	if (size & 0x2) {
		size >>= 0x01;
		rslt |=  0x01;
	}

	return rslt;
}

/****************************************************************************
 * heap_malloc
 *
 * Returns a pointer to an unused, read-write block of memory of at least the
 * specified size. This pointer can be free via heap_free. Returns null on
 * out of memory of too large error. This function is thread safe.
 */

void *heap_malloc(size_t size) {
	struct block *block;
	char  *slab;
	size_t i;

	/* convert size to bucket index */
	size += sizeof(size_t);
	size = (size < sizeof(struct block)) ? sizeof(struct block) : size;
	size = get_bucket(size);

	/* mutex bucket */
	mutex_spin(&m_bucket[size]);

	/* allocate more blocks if bucket empty */
	if (!bucket[size]) {
		mutex_spin(&m_brk);
		if (size < 12) {
			slab = brk;
			brk  = (void*) ((uintptr_t) brk + PAGESZ);

			mmap(slab, PAGESZ, PROT_READ | PROT_WRITE);

			i = PAGESZ - (1 << size);
			do {
				block = (struct block*) &slab[i];
				block->next = bucket[size];
				block->size = size;
				bucket[size] = block;

				i -= (1 << size);
			} while (i);
		}
		else {
			slab = brk;
			brk  = (void*) ((uintptr_t) brk + (1 << size));

			mmap(slab, 1 << size, PROT_READ | PROT_WRITE);

			block = (struct block*) ((uintptr_t) slab - sizeof(size_t));
			block->next = bucket[size];
			block->size = size;
			bucket[size] = block;
		}
		mutex_free(&m_brk);
	}

	/* allocate from bucket */
	block = bucket[size];
	bucket[size] = block->next;

	mutex_free(&m_bucket[size]);

	return (void*) ((uintptr_t) block + sizeof(size_t));
}

/****************************************************************************
 * heap_free
 *
 * Frees a given block back into the heap. This algorithm DOES NOT detect
 * double frees of out of bounds pointers - be careful. The function is
 * thread safe.
 */

void heap_free(void *ptr) {
	struct block *block;
	size_t size;

	block = (void*) ((uintptr_t) ptr - sizeof(size_t));
	size  = block->size;

	mutex_spin(&m_bucket[size]);
	block->next  = bucket[size];
	bucket[size] = block;
	mutex_spin(&m_bucket[size]);
}

/****************************************************************************
 * heap_size
 *
 * Returns the allocated size of a given heap block. This size may or may not
 * be the size given to heap_malloc, but is larger than or equal to it.
 */

size_t heap_size(void *ptr) {
	struct block *block;

	block = (void*) ((uintptr_t) ptr - sizeof(size_t));

	return (1 << block->size);
}

/****************************************************************************
 * heap_valloc
 *
 * Returns an allocated block of memory that is PAGESZ aligned. All other
 * behavior is identical to that of heap_malloc.
 */

void *heap_valloc(size_t size) {

	if (size < PAGESZ) {
		size = PAGESZ;
	}

	return heap_malloc(size);
}
