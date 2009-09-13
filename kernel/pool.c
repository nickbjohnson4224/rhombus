/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <mem.h>

__attribute__ ((section(".itext"))) 
pool_t *pool_new(uint32_t size) {
	uint32_t npool;
	pool_t *pool;

	npool = (size - 1) / 32 + 2; /* 32 entries per pool (and round up) */
	pool = kmalloc(sizeof(pool_t) * npool);
	memclr(pool, sizeof(pool_t) * npool);
	pool[0] = npool;

	return pool;
} 

static void pool_full(pool_t *pool) {
	printk("\npool %x\n", pool);
	panic("pool allocator full");
}

uint32_t pool_alloc(pool_t *pool) {
	uint32_t p, b;	/* pool, bit */

	if (!pool) panic("No pool");

	/* Find suitable pool */
	for (p = 1; p < pool[0] && pool[p] == 0xFFFFFFFF; p++);
	if (p == pool[0]) pool_full(pool);

	/* Find open bit within pool */
	for (b = 0; pool[p] & (0x1 << b); b++);

	/* Set bit */
	pool[p] |= (0x1 << b);

	return (p << 5) | b;
}

uint32_t pool_free(pool_t *pool, uint32_t pos) {
	uint32_t p, b;

	/* Convert to bit coordinates */
	p = (pos >> 5) + 1;
	b = pos % 32;

	/* Clear bit */
	pool[p] &= ~(0x1 << b);

	return 0;
}

uint32_t pool_query(pool_t *pool) {
	uint32_t total, p, b;
	
	/* Tally usage of all pools */
	total = 0;
	for (p = 1; p < pool[0]; p++)
		for (b = 0; b < 32; b++)
			if (pool[p] & (0x1 << b)) total++;

	return (uint32_t) total;
}
