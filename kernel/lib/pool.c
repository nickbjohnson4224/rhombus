// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

__attribute__ ((section(".ttext"))) 
pool_t *pool_new(uint32_t size, pool_t *pool) {
	uint32_t i, npool, extra;

	npool = (size - 1) / 1024 + 1; // 1024 entries per pool (and round up)
	extra = size - ((npool - 1) * 1024);
	
	for (i = 0; i < npool; i++) {
		memclr(pool[i].word, sizeof(uint32_t) * 32);
		pool[i].first = 0x0000;
		pool[i].total = (uint16_t) ((i < npool - 1) ? 1024 : extra);
		pool[i].setup = 0x4224;
		pool[i].upper = pool[i].total;
	}

	return pool;
}

static void pool_full(pool_t *pool) {
	printk("%x\n", pool_query(pool));
	panic("pool allocator full");
}

uint32_t pool_alloc(pool_t *pool) {
	uint32_t p, w, b;	// pool, word, bit

	// Find suitable pool
	for (p = 0; pool[p].total == 0; p++) 
		if (pool[p].setup != 0x4224) pool_full(pool);
	if (pool[p].setup != 0x4224) pool_full(pool);

	// Find suitable word within pool
	for (w = pool[p].first / 32; w < pool[p].upper / 32; w++)
		if (pool[p].word[w] != 0xFFFFFFFF) break;

	// Find open bit within word
	for (b = 0; pool[p].word[w] & (0x1 << b); b++);
	if (b == 32) pool_full(pool);

	pool[p].word[w] |= (0x1 << b);
	pool[p].total --;
	if (pool[p].first == ((w << 5) | b)) pool[p].first++;
	return ((p << 10) | ((w << 5) | b));
}

uint32_t pool_free(pool_t *pool, uint32_t pos) {
	uint32_t p, w, b;

	// Convert to bit coordinates
	p = pos >> 10;
	w = (pos >> 5) % 1024;
	b = pos % 32;

	// Clear bit and set metadata
	pool[p].word[w] &= ~(0x1 << b);
	pool[p].first = (uint16_t) min(pool[p].first, ((w << 5) | b));
	pool[p].total ++;

	return 0;
}

uint32_t pool_query(pool_t *pool) {
	int total, p;
	
	// Tally usage of all pools
	total = 0;
	for (p = 0; pool[p].setup; p++)
		total += (pool[p].upper - pool[p].total);

	return (uint32_t) total;
}
