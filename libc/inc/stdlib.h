#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

void init_heap(void);
void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

#endif/*STDLIB_H*/
