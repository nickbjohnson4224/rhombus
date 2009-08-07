// Copyright 2009 Nick Johnson

#ifndef LIB_H
#define LIB_H

#include <config.h>

typedef unsigned int u32int;
typedef unsigned short u16int;
typedef unsigned char u8int;
typedef unsigned int addr_t;

/***** STANDARD FUNCTIONS *****/
void outb(u16int port, u8int val);
u8int inb(u16int port);
#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)
void sleep(u32int cycles);
#define NULL ((void*) 0)

/***** STRING FUNCTIONS *****/
void *memcpy(void *dest, void *src, u32int size);
void *memset(void *dest, u8int src, u32int size);
void *memclr(void *dest, u32int size);
void *pgclr(u32int *base);
char *strcpy(char *dest, char *src);
int strcmp(char *s1, char *s2);
u32int strlen(char *str);
u32int atoi(char *str, u8int base);
char *itoa(u32int n, char *buf, u8int base);

/***** OUTPUT FUNCTIONS *****/
void cleark();
void printk(char *fmt, ...);
void colork(u8int color);
void cursek(u8int x, u8int y);

/***** ERROR FUNCTIONS *****/
void panic(char *message);		// Panic with a message

#define ENOTASK (-1)
#define ENOSIG (-2)
#define EPERMIT (-3)
#define EREPEAT (-4)

/***** POOL ALLOCATOR *****/

typedef struct pool {
	u32int word[32];
	u16int first;	// First free bit in pool
	u16int total;	// Total free bits in pool
	u16int setup;	// Magic number that verifies setup
	u16int upper;	// Highest actual bit in pool
} pool_t;

pool_t *pool_new(u32int num, pool_t *pool);	// Allocate a new pool
u32int pool_alloc(pool_t *pool);			// Allocate from a pool
u32int pool_free(pool_t *pool, u32int pos);	// Free back to a pool
u32int pool_query(pool_t *pool);			// Find how many elementa are allocated in a pool

#endif
