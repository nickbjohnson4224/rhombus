/* Copyright 2010 Nick Johnson */

#ifndef HEAP_H
#define HEAP_H

#include <flux/arch.h>
#include <flux/request.h>

void  heap_init(void);

void  *heap_valloc(size_t size);
void  *heap_aalloc(size_t size, size_t align);
void  *heap_malloc(size_t size);
void   heap_free(void *ptr);
size_t heap_size(void *ptr);

int    heap_check(void *ptr);

#define HCHECK_RANGE	0	/* pointer not within heap boundaries */
#define HCHECK_ALLOC	1	/* pointer at start of allocated block */
#define HCHECK_FREED	2	/* pointer at start of freed block */
#define HCHECK_ALIGN	3	/* pointer not at start of block */

req_t *ralloc(void);
void   rfree(req_t *req);

#endif
