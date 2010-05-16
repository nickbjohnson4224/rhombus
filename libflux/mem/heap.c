/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/heap.h>
#include <flux/mmap.h>

#define SBLOCK_SIZE	0x10000

/****************************************************************************
 * block
 *
 * Represents a single free block in the heap. The corresponding superblock
 * can be calculated using get_sb().
 */

struct block {
	struct block *next;
};

/****************************************************************************
 * sblock
 *
 * Represents a single superblock in the heap.
 */

struct sblock {
	struct block  *free;
	uint32_t       mutex; 
	struct sblock *next;
	uint16_t       size;
	uint16_t       refc;
};

/****************************************************************************
 * brk
 *
 * Top of heap memory.
 */

static void      *brk = (void*) HEAP_START;
static uint32_t m_brk;

/****************************************************************************
 * sb_freelist
 *
 * List of free superblocks in the heap.
 */

static struct sblock *sb_freelist;
static uint32_t     m_sb_freelist;

/****************************************************************************
 * bucket
 *
 * Lists of superblocks containing blocks of size (1 << index).
 */

static struct sblock *bucket[16];
static uint32_t     m_bucket[16];

/****************************************************************************
 * salloc
 *
 * Allocates a superblock. First checks the free superblock list, then expands
 * the heap. Returns a pointer to a single unused superblock header pointing
 * to readable and writable memory on success, null on out of memory error. 
 * The superblock has its mutex set to free. This function is thread safe but
 * not lock-free.
 */

static void *salloc() {
	struct sblock *sblock;
	uintptr_t base;

	if (sb_freelist) {
		mutex_spin(&m_sb_freelist);

		sblock = sb_freelist;
		sb_freelist = sb_freelist->next;

		sblock->mutex = 0;
		sblock->next  = NULL;
		sblock->size  = 0;
		sblock->refc  = 0;

		mutex_free(&m_sb_freelist);
	}
	else {
		mutex_spin(&m_brk);

		if ((uintptr_t) brk + SBLOCK_SIZE > HEAP_MXBRK) {
			sblock = NULL;
		}
		else {
			base   = (uintptr_t) brk;
			brk    = (void*) (base + SBLOCK_SIZE);
			sblock = (void*) (base + SBLOCK_SIZE - sizeof(struct sblock));

			mmap((void*) base, SBLOCK_SIZE, PROT_READ | PROT_WRITE);

			sblock->free  = (void*) base;
			sblock->mutex = 0;
			sblock->next  = NULL;
			sblock->size  = 0;
			sblock->refc  = 0;
		}

		mutex_free(&m_brk);
	}

	return sblock;
}

/****************************************************************************
 * sfree
 *
 * Frees a superblock, adding it to the free superblock list. This function
 * is thread safe but not lock-free.
 */

static void sfree(struct sblock *sblock) {

	mutex_spin(&m_sb_freelist);

	sblock->next = sb_freelist;
	sb_freelist = sblock;

	mutex_free(&m_sb_freelist);
}

/****************************************************************************
 * get_sb
 *
 * Returns a pointer to the superblock header corresponding to a block.
 */

static struct sblock *get_sb(struct block *b) {
	uintptr_t addr;

	addr = (uintptr_t) b;
	addr = addr - (addr % SBLOCK_SIZE);
	addr = addr + SBLOCK_SIZE - sizeof(struct sblock);

	return (struct sblock*) addr;
}

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
 * specified size. This pointer can be freed via heap_free. Returns null on
 * out of memory or too large error. This function is thread safe and 
 * lock-free locally.
 */

void *heap_malloc(size_t size) {
	struct sblock *sblock;
	struct block  *block;
	size_t i;

	/* convert size to bucket index */
	size = get_bucket(size);
	
	/* reject oversized allocations */
	if ((1 << size) > SBLOCK_SIZE) {
		return NULL;
	}

	/* correct undersized allocations */
	if ((1 << size) < sizeof(void*)) {
		size = get_bucket(sizeof(void*));
	}

	mutex_spin(&m_bucket[size]);
	sblock = bucket[size];

	/* search for unlocked, usable superblock in bucket */
	while (sblock) {
		if (mutex_lock(&sblock->mutex)) {
			if (!sblock->free) {
				mutex_free(&sblock->mutex);
			}
			else {
				break;
			}
		}
		else {
			sblock = sblock->next;
		}
	}

	/* allocate new superblock if none found */
	if (!sblock) {
		sblock = salloc();

		if (!sblock) {
			mutex_free(&m_bucket[size]);
			return NULL;
		}

		sblock->next = NULL;
		
		block = sblock->free;
		while ((uintptr_t) block + (1 << size) < (uintptr_t) sblock) {
			block->next = (void*) ((uintptr_t) block + (1 << size));
			block = block->next;
		}
		block->next = NULL;

		mutex_lock(&sblock->mutex);

		sblock->next = bucket[size];
		bucket[size] = sblock;
	}

	mutex_free(&m_bucket[size]);

	/* allocate from superblock */
	block = sblock->free;
	sblock->free = block->next;
	sblock->refc++;

	mutex_free(&sblock->mutex);

	return block;
}

/****************************************************************************
 * heap_free
 *
 * Frees a given block back into the heap. This algorithm DOES NOT detect
 * double frees or out of bounds pointers - be careful. This function is 
 * thread safe but not lock-free.
 */

void heap_free(void* ptr) {
	struct sblock *sblock, *sblock0;
	struct block  *block;
	size_t size;

	block  = ptr;
	sblock = get_sb(block);

	mutex_spin(&sblock->mutex);

	block->next  = sblock->free;
	sblock->free = block;
	sblock->refc--;

	if (sblock->refc == 0) {
		size = sblock->size;
		size = get_bucket(size);

		mutex_spin(&m_bucket[size]);

		sblock0 = bucket[size];

		if (sblock0 == sblock) {
			bucket[size] = sblock0->next;
		}
		else while (sblock0) {
			if (sblock0->next == sblock) {
				sblock0->next = sblock->next;
				break;
			}
			else {
				sblock0 = sblock0->next;
			}
		}

		mutex_free(&m_bucket[size]);

		sfree(sblock);
	}
	else {
		mutex_free(&sblock->mutex);
	}
}

/****************************************************************************
 * heap_size
 *
 * Returns the allocated size of a given heap block. This size may or may not
 * be the size given to heap_malloc, but is larger than or equal to it.
 */

size_t heap_size(void *ptr) {
	return (get_sb(ptr))->size;
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
