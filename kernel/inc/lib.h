// Copyright 2009 Nick Johnson

#ifndef LIB_H
#define LIB_H

#include <config.h>
#include <stdint.h>

//typedef unsigned int uint32_t;
//typedef unsigned short uint16_t;
//typedef unsigned char uint8_t;
typedef unsigned int addr_t;

/***** STANDARD FUNCTIONS *****/
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)
void sleep(uint32_t cycles);
#define NULL ((void*) 0)

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

/***** OUTPUT FUNCTIONS *****/
void cleark(void);
void printk(const char *fmt, ...);
void colork(uint8_t color);
void cursek(int8_t x, int8_t y);

/***** ERROR FUNCTIONS *****/
void panic(const char *message);		// Panic with a message

#define ENOTASK ((uint32_t) -1)
#define ENOSIG ((uint32_t) -2)
#define EPERMIT ((uint32_t) -3)
#define EREPEAT ((uint32_t) -4)

/***** POOL ALLOCATOR *****/

#define SIZEOF_POOL(n) (((n) / 1024) + 1)

typedef struct pool {
	uint32_t word[32];
	uint16_t first;	// First free bit in pool
	uint16_t total;	// Total free bits in pool
	uint16_t setup;	// Magic number that verifies setup
	uint16_t upper;	// Highest actual bit in pool
} pool_t;

pool_t *pool_new(uint32_t num, pool_t *pool);		// Allocate a new pool
uint32_t pool_alloc(pool_t *pool);					// Allocate from a pool
uint32_t pool_free(pool_t *pool, uint32_t pos);		// Free back to a pool
uint32_t pool_query(pool_t *pool);					// Find how many elementa are allocated in a pool

#endif
