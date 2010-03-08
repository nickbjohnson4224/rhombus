/* Copyright 2009 Nick Johnson */

#ifndef LIB_H
#define LIB_H

#include <flux/config.h>
#include <flux/arch.h>

void sleep(uint32_t cycles);

/***** STRING FUNCTIONS *****/
void *memcpy(void *dest, void *src, uint32_t size);
void *memset(void *dest, uint8_t src, uint32_t size);
void *memclr(void *dest, uint32_t size);
void *pgclr(uint32_t *base);
char *strcpy(char *dest, char *src);
int strcmp(char *s1, char *s2);
uint32_t strlen(char *str);
int atoi(char *str, uint8_t base);
char *itoa(uint32_t n, char *buf, uint8_t base);

/***** POOL ALLOCATOR *****/

#define SIZEOF_POOL(n) (((n) / 1024) + 1)

typedef uint32_t pool_t;

pool_t *pool_new(uint32_t num);						/* Allocate a new pool */
uint32_t pool_alloc(pool_t *pool);					/* Allocate from a pool */
uint32_t pool_free(pool_t *pool, uint32_t pos);		/* Free back to a pool */
uint32_t pool_query(pool_t *pool);					/* Find how much of a pool is full */

/***** STACK ALLOCATOR *****/

void *kmalloc_align(uint32_t size, uint32_t align);
void *kmalloc(uint32_t size);

#endif
