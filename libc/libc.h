#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>
#include <stdbool.h>
#include <flux.h>

/*** Memory Management ***/

int32_t __mmap(uint32_t addr, uint32_t size, uint32_t flags);
int32_t __umap(uint32_t addr, uint32_t size);
int32_t __emap(uint32_t addr, uint32_t frame, uint32_t flags);

/*** Heap ***/

struct _heap_allocator {
	size_t max_size;
	void (*init)(void);		/* Initialize allocator */
	void*(*alloc)(size_t);	/* Allocate chunk */
	void (*free)(void*);	/* Free chunk */
	bool (*cont)(void*);	/* Check if is in charge of chunk */
	size_t(*size)(void*);	/* Check size of chunk */
};

void   _heap_init(void);
void  *_heap_alloc(size_t size);
void   _heap_free(void *ptr);
size_t _heap_size(void *ptr);

struct request *_heap_req_alloc(void);
void _heap_req_free(struct request *);

#endif
