// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <init.h>

// Assume at least 4MB of memory, because we can (it's a 386+)
u32int memsize = 0x400000;
u32int proto_base = KSPACE + 0x400000;

#ifdef TEST
void test_mem() {
	u32int i;	

	// Find number of used frames
	printk("%x frames used\n", pool_query(fpool));

	// Allocate 200 frames
	for (i = 0x42240000; i < 0x44220000; i += 0x1000)
		p_alloc(i, (PF_PRES | PF_RW));
	
	// Find number of used frames
	printk("%x frames used\n", pool_query(fpool));

	// Free 200 frames in order
	for (i = 0x42240000; i < 0x44220000; i += 0x1000)
		p_free(i);

	// Find number of used frames
	printk("%x frames used\n", pool_query(fpool));

	// Clone a few tables
	map_t maps[42];
	for (i = 0; i < 42; i++) maps[i] = map_clone();

}
#endif

__attribute__ ((section(".ttext"))) 
void init_mem() {
	u32int i, *temp, initrd_end;
	printk("  Kernel: allocators");

		// Initialize frame allocator
		pool_new(memsize >> 12, fpool);

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: memory map");

		// Make a new and proper memory map for the kernel in the init map
		extern u32int init_kmap;
		u32int *kmap = &init_kmap;
		temp = (void*) (KSPACE + 0x1000);
		pgclr(temp);
		temp[1023] = 0x1000 | (PF_PRES | PF_RW);
		kmap[1022] = 0x1000 | (PF_PRES | PF_RW);
		map_load((u32int) kmap - KSPACE);

		// Identity map necessary kernel memory (i.e. code and initrd)
		extern u32int init_ktbl;
		tmap[KSPACE >> 22] = ((u32int) &init_ktbl - KSPACE) | (PF_PRES | PF_RW);
		initrd_end = *(u32int*) (mboot->mods_addr + KSPACE + 4) + KSPACE;
		for (i = KSPACE; i < initrd_end; i += 0x1000)
			ttbl[i >> 12] = frame_new() | (PF_PRES | PF_RW);

		// Reload the new map
		map_load(0x1000);

	cursek(74, -1);
	printk("[done]");
}

void init_free() {
	u32int i, base, limit, freed = 0;
	printk("  Kernel: GC: ");

		// Free marked code and global data
		extern u32int START_OF_TEMP;
		base = (u32int) &START_OF_TEMP;
		extern u32int END_OF_TEMP;
		limit = (u32int) &END_OF_TEMP;
		for (i = base; i < (limit & ~0xFFF); i += 0x1000, freed++) p_free(i);
		
		// Free initrd image data
		extern u32int end;
		i = ((u32int) &end + 0x1000) & ~0xFFF;
		for (; i < KSPACE + 0x200000; i += 0x1000) {
			if (page_get(i) & PF_PRES) {
				frame_free(page_ufmt(page_get(i)));
				freed++;
			}
		}

		// Free unused (by BIOS) lower memory
		for (i = KSPACE + 0x1000; i < KSPACE + 0x80000; i += 0x1000, freed++) p_free(i);
		
		u32int percent_used = (pool_query(fpool) * 40000) / (memsize >> 10);
		printk("%d KB freed; %d.%d%% used", freed * 4, percent_used / 100, percent_used % 100);

	cursek(74, -1);
	printk("[done]");
}
