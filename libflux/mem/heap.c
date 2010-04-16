/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/config.h>
#include <flux/heap.h>
#include <flux/signal.h>
#include <flux/mmap.h>

/* abmalloc - The AfterBurner Memory Allocator
 * Copyright 2010 Nick Johnson
 *
 * abmalloc is a two layer virtual memory allocator.
 * The first layer is an implementation of valloc(),
 * which only allocates page aligned areas of memory,
 * which is a fast and easy operation. It uses only
 * a bitmap of which pages are free. Any request of
 * more than half a block is handled by this allocator.
 *
 * The second layer is a slab allocator, which creates
 * pools of quickly allocated fixed sized chunks as
 * they are requested. This reduces the allocation time
 * for a frequently used size two or threefold over
 * algorithms like dlmalloc, and makes it guaranteed
 * constant time.
 *
 * The focus of this allocator's performance is speed
 * over memory efficency. However, for allocations of
 * very small structures, like 2 or 4 dwords, there is
 * less than 0.5% memory overhead, unlike dlmalloc,
 * which can have up to a 50% overhead. 
 *
 * There is a guarantee of no memory fragmentation in 
 * a strict sense, although if allocations are not 
 * predicatble, a lot of memory will go unused. 
 * Allocating one of each multiple of 8 bytes below 
 * 4096 will take up 256 KB!
 *
 * To take full advantage of this memory allocator, try
 * to allocate as many similar structures as possible,
 * and avoid its pathological memory use cases. It is
 * astoundingly fast under the right conditions.
 */

#define BLOCKSZ PAGESZ
#define _HEAP_SIZE (HEAP_MXBRK - HEAP_START)
#define _BMAP_SIZE (_HEAP_SIZE / (BLOCKSZ * 8))

#define _BMAP_START HEAP_START
#define _HEAP_START (HEAP_START + _BMAP_SIZE)

/*** Helper Routines / Structures ***/

struct free_block {
	struct free_block *next;
};

struct slab_header {
	struct slab_header *next; /* Next slab in list */
	struct free_block *freel; /* Free element list */ 
	uint32_t magic;	/* Magic number for ID - 0x42242442 */
	uint16_t refc;  /* Reference count - number of allocated elements */
	uint16_t esize; /* Element size */
};

static uint16_t *bmap = (void*) _BMAP_START;
static size_t bmap_top = 0;
static struct slab_header *bucket[BLOCKSZ / sizeof(void*)];
static struct slab_header *slab_deathrow = NULL;

static uint32_t heap_mutex = 0;

static void heap_vfree(void* ptr);

/*** Slab Allocator ***/

static void *new_slab(size_t size) {
	struct slab_header *slab; 
	struct free_block *b;
	
	if (slab_deathrow && slab_deathrow->esize == size) {
		slab = slab_deathrow;
		slab_deathrow = NULL;
	}
	else {
		slab = heap_valloc(BLOCKSZ);
	}

	b = (void*) ((uintptr_t) slab + sizeof(struct slab_header));

	slab->next = NULL;
	slab->freel = b;
	slab->magic = 0x42242442;
	slab->refc = 0;
	slab->esize = size;

	while ((uintptr_t) b + size < (uintptr_t) slab + BLOCKSZ) {
		b->next = (void*) ((uintptr_t) b + size);
		b = (void*) ((uintptr_t) b + size);
	}
	b->next = NULL;

	return slab;
}

static void del_slab(struct slab_header *slab) {
	struct slab_header *s;

	s = slab_deathrow;
	slab_deathrow = slab;

	if (s) heap_vfree(s);
}

static struct slab_header *get_slab(void *ptr) {
	struct slab_header *h = (void*) ((uintptr_t) ptr &~ 0xFFF);

	if (h->magic != 0x42242442) {
		return NULL;
	}
	else {
		return h;
	}
}

static void *slab_alloc(struct slab_header *slab) {
	struct free_block *b;

	if (!slab->freel) {
		return NULL;
	}

	b = slab->freel;
	slab->freel = b->next;
	slab->refc++;

	return b;
}

static void slab_free(struct slab_header *slab, void *ptr) {
	struct free_block *b = ptr;

	b->next = slab->freel;
	slab->freel = b;
	slab->refc--;
}

/*** Block Allocator (vmalloc) ***/

void *heap_valloc(size_t size) {
	size_t idx = 0, i;
	uintptr_t addr;
	size_t nblocks = (size % BLOCKSZ) ? (size / BLOCKSZ) + 1 : size / BLOCKSZ;

	if (size > BLOCKSZ) {
		idx = bmap_top;
		
		for (i = 0; i < nblocks; i++, idx++) {
			bmap[idx >> 4] |= (1 << (idx & 0xF));
		}

		bmap_top += nblocks;
	}
	else {
		while (idx < _HEAP_SIZE) {
			if (bmap[idx >> 4] == 0xFFFF) {
				idx += 16;
				continue;
			}
			if ((bmap[idx >> 4] & (1 << (idx & 0xF))) == 0) {
				break;
			}
			idx++;
		}

		if (idx >= bmap_top) bmap_top = idx + 1;
		bmap[idx >> 4] |= (1 << (idx & 0xF));
	}
	
	addr = _HEAP_START + (idx * BLOCKSZ);
	mmap((void*) addr, size, MMAP_READ | MMAP_WRITE);

	return (void*) addr;
}

static void heap_vfree(void *ptr) {
	size_t idx = ((uintptr_t) ptr - _HEAP_START) / BLOCKSZ;

	bmap[idx >> 4] &= ~(1 << (idx & 0xF));
	umap(ptr, BLOCKSZ);
}

/*** The Heap Interface ***/

void heap_init(void) {
	size_t i;

	mmap((void*) _BMAP_START, _BMAP_SIZE, MMAP_WRITE | MMAP_READ);
	
	for (i = 0; i < _BMAP_SIZE / sizeof(uint16_t); i++) {
		bmap[i] = 0;
	}
}

struct request *ralloc(void) {
	void *ptr;

	mutex_spin(&heap_mutex);

	ptr = heap_valloc(PAGESZ);

	mutex_free(&heap_mutex);

	return ptr;
}

void rfree(struct request *r) {
	size_t idx;

	idx = ((uintptr_t) r - _HEAP_START) / BLOCKSZ;

	mutex_spin(&heap_mutex);

	bmap[idx >> 4] &= ~(1 << (idx & 0xF));

	mutex_free(&heap_mutex);
}

void *heap_malloc(size_t size) {
	struct slab_header *slab;
	size_t bidx;
	void *ptr;

	mutex_spin(&heap_mutex);

	if (size > (BLOCKSZ / 2) - sizeof(struct slab_header)) {
		ptr = heap_valloc(size);
		mutex_free(&heap_mutex);
		return ptr;
	}
	
	if (size % sizeof(void*) != 0) {
		size = (size - (size % sizeof(void*))) + sizeof(void*);
	}
	bidx = size / sizeof(void*);

	slab = bucket[bidx];
	while (slab) {
		if (slab->freel) break;
		slab = slab->next;
	}

	if (slab == NULL) {
		slab = new_slab(size);
		slab->next = bucket[bidx];
		bucket[bidx] = slab;
	}

	ptr = slab_alloc(slab);
	mutex_free(&heap_mutex);
	return ptr;
}

void heap_free(void *ptr) {
	struct slab_header *s, *x;
	size_t bidx;

	mutex_spin(&heap_mutex);

	if ((uintptr_t) ptr < _HEAP_START || (uintptr_t) ptr > HEAP_MXBRK) {
		mutex_free(&heap_mutex);
		return;
	}

	s = get_slab(ptr);

	if (s == NULL) {
		heap_vfree(ptr);
		mutex_free(&heap_mutex);
		return;
	}

	slab_free(s, ptr);

	if (s->refc == 0) {
		bidx = s->esize / sizeof(void*);
		x = bucket[bidx];
		if (x == s) {
			bucket[bidx] = x->next;
		}
		else {
			while (x && x->next) {
				if (x->next == s) {
					x->next = s->next;
					break;
				}
				x = x->next;
			}
		}
		del_slab(s);
	}

	mutex_free(&heap_mutex);
}

size_t heap_size(void *ptr) {
	struct slab_header *slab;
	size_t size;

	mutex_spin(&heap_mutex);

	slab = get_slab(ptr);

	if (slab == NULL) {
		size = BLOCKSZ;
	}
	else {
		size = slab->esize;
	}
	
	mutex_free(&heap_mutex);

	return size;
}
