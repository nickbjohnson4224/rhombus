#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>

/*** Libc Heap */

void   _heap_init(void);
void  *_heap_alloc(size_t size);
void   _heap_free(void *ptr);
size_t _heap_size(void *ptr);

#endif
