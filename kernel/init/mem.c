// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>
#include <mem.h>

// Assume at least 4MB of memory, because we can (it's a 386+)
uint32_t memsize = 0x400000;
uint32_t proto_base = KSPACE + 0x400000;

__attribute__ ((section(".ttext"))) 
void init_mem() {
	uint32_t i, *temp, initrd_end;
	printk("  Kernel: allocators");

		// Initialize frame allocator
		pool_new(memsize >> 12, fpool);

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: memory map");

		// Make a new and proper memory map for the kernel in the init map
		extern uint32_t init_kmap;
		uint32_t *kmap = &init_kmap;
		temp = (void*) (KSPACE + 0x1000);
		pgclr(temp);
		temp[1023] = 0x1000 | (PF_PRES | PF_RW);
		kmap[1022] = 0x1000 | (PF_PRES | PF_RW);
		map_load((uint32_t) kmap - KSPACE);

		// Identity map necessary kernel memory (i.e. code and initrd)
		extern uint32_t init_ktbl;
		tmap[KSPACE >> 22] = ((uint32_t) &init_ktbl - KSPACE) | (PF_PRES | PF_RW);
		initrd_end = *(uint32_t*) (mboot->mods_addr + KSPACE + 4) + KSPACE;
		for (i = KSPACE; i < initrd_end; i += 0x1000)
			ttbl[i >> 12] = page_fmt(frame_new(), (PF_PRES | PF_RW));

		// Reload the new map
		map_load(0x1000);

	cursek(74, -1);
	printk("[done]");
}

void init_free() {
	uint32_t i, base, limit, freed = 0;
	printk("  Kernel: GC: ");

		// Free marked code and global data
		extern uint32_t START_OF_TEMP;
		base = (uint32_t) &START_OF_TEMP;
		extern uint32_t END_OF_TEMP;
		limit = (uint32_t) &END_OF_TEMP;
		for (i = base; i < (limit & ~0xFFF); i += 0x1000, freed++) p_free(i);
		
		// Free initrd image data
		extern uint32_t end;
		i = ((uint32_t) &end + 0x1000) & ~0xFFF;
		for (; i < KSPACE + 0x200000; i += 0x1000) {
			if (page_get(i) & PF_PRES) {
				frame_free(page_ufmt(page_get(i)));
				freed++;
			}
		}

		// Free unused (by BIOS) lower memory
		for (i = KSPACE + 0x1000; i < KSPACE + 0x80000; i += 0x1000, freed++) p_free(i);

		printk("%d KB freed; %d KB used", freed * 4, pool_query(fpool) * 4);

	cursek(74, -1);
	printk("[done]");
}
