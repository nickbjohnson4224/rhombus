/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex.h>
#include <arch.h>
#include <mmap.h>

struct block {
	size_t size;
	struct block *next;
};

static void *brk = (void*) HEAP_START;
static bool m_brk;

static struct block *bucket[sizeof(uintptr_t) << 3];
static bool        m_bucket[sizeof(uintptr_t) << 3];

/****************************************************************************
 * get_bucket
 *
 * Returns an index corresponding to the appropriate bucket for the given
 * block size. Uses a portable, unrolled version of the algorithm detailed
 * at <http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog>.
 */

static size_t get_bucket(size_t size) {
	register size_t rslt = 0;
	size_t osize = size;

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

	return (((size_t) 1 << rslt) == osize) ? rslt : rslt + 1;
}

/****************************************************************************
 * malloc
 *
 * Returns a pointer to an unused, read-write block of memory of at least the
 * specified size. This pointer can be free via heap_free. Returns null on
 * out of memory of too large error. This function is thread safe.
 */

void *malloc(size_t size) {
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
			slab   = brk;
			brk = (void*) ((uintptr_t) brk + PAGESZ);

			mmap(slab, PAGESZ, PROT_READ | PROT_WRITE);

			i = PAGESZ - (1 << size);
			do {
				block = (struct block*) &slab[i];
				block->next  = bucket[size];
				block->size  = size;
				bucket[size] = block;

				i -= (1 << size);
			} while (i);
		}
		else {
			slab = brk;
			brk  = (void*) ((uintptr_t) brk + (1 << size));

			mmap(slab, 1 << size, PROT_READ | PROT_WRITE);

			block = (struct block*) slab;
			block->next  = bucket[size];
			block->size  = size;
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

/***************************************************************************
 * free
 *
 * Frees a given block back into the heap. This algorithm DOES NOT detect
 * double frees of out of bounds pointers - be careful. The function is
 * thread safe.
 */

void free(void *ptr) {
	struct block *block;
	size_t size;

	if (!((uintptr_t) ptr % PAGESZ)) {
		return;
	}

	block = (void*) ((uintptr_t) ptr - sizeof(size_t));
	size  = block->size;

	mutex_spin(&m_bucket[size]);
	block->next  = bucket[size];
	bucket[size] = block;
	mutex_spin(&m_bucket[size]);
}

/****************************************************************************
 * msize
 *
 * Returns the allocated size of a given heap block. This size may or may not
 * be the size given to malloc, but is larger than or equal to it.
 */

size_t msize(void *ptr) {
	struct block *block;

	block = (void*) ((uintptr_t) ptr - sizeof(size_t));

	return (1 << block->size);
}

/****************************************************************************
 * aalloc
 *
 * Returns an allocated block of memory that is <align> aligned. All other
 * behavior is identical to that of malloc.
 */

void *aalloc(size_t size, size_t align) {
	uintptr_t addr;

	if (size < PAGESZ) {
		size = PAGESZ;
	}

	addr = (uintptr_t) malloc(size);
	addr -= addr % PAGESZ;

	return (void*) addr;
}
