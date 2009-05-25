// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <init.h>

// Assume at least 4MB of memory, because we can (it's a 386+)
u32int memsize = 0x400000;
u32int proto_base = 0xF8010000; // Use up the lower memory for allocation if possible

__attribute__ ((section(".ttext"))) 
static void *init_kmalloc(u32int size) {
	u32int temp;
	if (proto_base & 0xFFF) proto_base = (proto_base & ~0xFFF) + 0x1000;
	temp = proto_base;
	proto_base += size;
	if (proto_base >= 0xF8080000) panic("initial allocator overrun\n");
	return (void*) temp;
}

__attribute__ ((section(".ttext"))) 
static page_t page_touch_init(map_t *map, u32int page) {
	if (map->virt[page >> 22]) return map->virt[page >> 22][(page >> 12) % 1024];
	map->virt[page >> 22] = init_kmalloc(0x1000);
	map->pdir[page >> 22] = phys_of(&kmap, (void*) map->virt[page >> 22]) | 0x7;
	pgclr(map->virt[page >> 22]);
	return 0x00000000;
}

__attribute__ ((section(".ttext"))) 
static map_t *map_alloc_init(map_t *map) {
	map->pdir = init_kmalloc(0x1000);
	map->virt = init_kmalloc(0x1000);
	map->cache = phys_of(&kmap, map->pdir);
	memclr((u32int*) map->pdir, 0x1000);
	memclr((u32int*) map->virt, 0x1000);
	return map;
}

__attribute__ ((section(".ttext"))) 
static pool_t *pool_new_init(u32int num) {
	pool_t *pool;
	u32int i, npool, extra;

	npool = (num - 1) / 1024 + 1; // 1024 entries per pool (and round up)
	extra = num - ((npool - 1) * 1024);
	pool = init_kmalloc(sizeof(pool_t) * npool);
	
	for (i = 0; i < npool; i++) {
		memclr(pool[i].word, sizeof(u32int) * 32);
		pool[i].first = 0x0000;
		pool[i].total = (i != npool - 1) ? 1024 : extra;
		pool[i].setup = 0x4224;
		pool[i].upper = pool[i].total;
	}

	return pool;
}

__attribute__ ((section(".ttext"))) 
void init_mem() {
	u32int i;
	colork(0xC);
	printk("Allocators");

		// Make a new and proper memory map for the kernel
		map_alloc_init(&kmap);

		// Initialize frame allocator
		fpool = pool_new_init(memsize >> 12);

		// Create the address space allocator
		ppool = pool_new_init(0x07C00000 >> 12);	

		// Preallocate page tables for the address space allocator
		for (i = 0xF8400000; i; i += 0x400000) page_touch_init(&kmap, i);

	cursek(36, -1);
	printk("done\n");
	printk("Kernel map");

		// Identity map necessary kernel memory (i.e. code and initrd)
		i = 0xF8000000;
		while (i < (u32int) 0xF8400000) {
			page_touch_init(&kmap, i);
			page_t new_page = page_fmt(frame_new(), (PF_PRES | PF_RW));
			page_set(&kmap, i, new_page);
			i += 0x1000;
		}

		// Reload the new map by going through physical memory
		kmap.pdir[0] = kmap.pdir[0xF8000000 >> 22];
		extern void redo_paging(u32int);
		redo_paging(phys_of(&kmap, kmap.pdir));
		kmap.pdir[0] = 0x00000000;

		// Mark memory as set up, so the new allocator is used
		mem_setup = 1;

	cursek(36, -1);
	printk("done\n");
	colork(0xF);
}

void init_free() {
	u32int i, base, limit;
	colork(0xC);
	printk("Temporary memory");

		// Free marked code and global data
		extern u32int START_OF_TEMP;
		base = (u32int) &START_OF_TEMP;
		extern u32int END_OF_TEMP;
		limit = (u32int) &END_OF_TEMP;
		for (i = base; i < (limit & ~0xFFF); i += 0x1000) p_free(&kmap, i);

		// Free initrd image data
		extern u32int end;
		i = ((u32int) &end + 0x1000) & ~0xFFF;
		for (; i < 0xF8400000; i += 0x1000)
			if (page_get(&kmap, i)) frame_free(page_ufmt(page_get(&kmap, i)));

	cursek(36, -1);
	printk("done\n");
	colork(0xF);
}
