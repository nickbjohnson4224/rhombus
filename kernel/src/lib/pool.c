// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>

pool_t *pool_new(u32int num) {
	pool_t *pool;
	u32int i, npool, extra;

	npool = (num - 1) / 1024 + 1; // 1024 entries per pool (and round up)
	extra = num - ((npool - 1) * 1024);
	pool = kmalloc(sizeof(pool_t) * npool);
	
	for (i = 0; i < npool; i++) {
		memclr(pool[i].word, sizeof(u32int) * 32);
		pool[i].first = 0x0000;
		pool[i].total = (i != npool - 1) ? 1024 : extra;
		pool[i].setup = 0x4224;
		pool[i].upper = pool[i].total;
	}

	return pool;
}

u32int pool_alloc(pool_t *pool) {
	u32int p, w, b;	// pool, word, bit

	// Find suitable pool
	for (p = 0; !pool[p].total; p++) 
		if (pool[p].setup != 0x4224) 
			panic("pool allocator full");

	// Find suitable word within pool
	for (w = pool[p].first / 32; w < pool[p].upper / 32; w++)
		if (pool[p].word[w] != 0xFFFFFFFF) break;

	// Find open bit within word
	for (b = 0; pool[p].word[w] & (0x1 << b); b++);
	if (b == 32) panic("pool allocator full");

	pool[p].word[w] |= (0x1 << b);
	pool[p].total --;
	if (pool[p].first == (w << 5) | b) pool[p].first++;
	return ((p << 10) | (w << 5) | b);
}

u32int pool_free(pool_t *pool, u32int pos) {
	u32int p, w, b;

	// Convert to bit coordinates
	p = pos >> 10;
	w = (pos >> 5) % 1024;
	b = pos % 32;

	// Clear bit and set metadata
	pool[p].word[w] &= ~(0x1 << b);
	pool[p].first = min(pool[p].first, (w << 5) | b);
	pool[p].total ++;

	return 0;
}
