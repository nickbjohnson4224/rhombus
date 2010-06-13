#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

void init_heap(void);
void *calloc(size_t nmemb, size_t size);
void *valloc(size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

int atoi(const char *nptr);

char *getenv(const char *name);
int   setenv(const char *name, const char *value, int overwrite);
int   unsetenv(const char *name);

#endif/*STDLIB_H*/
