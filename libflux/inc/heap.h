/* Copyright 2010 Nick Johnson */

#ifndef HEAP_H
#define HEAP_H

#include <flux/arch.h>

void  *heap_valloc(size_t size);
void  *heap_malloc(size_t size);
void   heap_free(void *ptr);
size_t heap_size(void *ptr);

void *high_malloc(size_t size);
void  high_free(void *ptr);

#endif
