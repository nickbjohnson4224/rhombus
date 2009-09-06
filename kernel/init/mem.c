// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>
#include <mem.h>

uint32_t memsize;

__attribute__ ((section(".itext"))) 
void init_mem() {
	uint32_t i, *temp, initrd_end;
	printk("  Kernel: allocators");

		// Initialize frame allocator
		fpool = pool_new(memsize >> 12);

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
		extern uint32_t allocator_space;
		for (i = KSPACE; i < allocator_space; i += 0x1000)
			ttbl[i >> 12] = page_fmt(frame_new(), (PF_PRES | PF_RW));

		// Reload the new map
		map_load(0x1000);

	cursek(74, -1);
	printk("[done]");
}

void init_free() {
	uint32_t base, limit;
	printk("  Kernel: GC: ");

		// Free marked code and global data
		extern uint32_t START_OF_INIT;
		base = (uint32_t) &START_OF_INIT;
		extern uint32_t END_OF_INIT;
		limit = (uint32_t) &END_OF_INIT;
		mem_free(base, limit - base);

		// Free initrd image data
		extern uint32_t end;
		mem_free(((uintptr_t) &end + 0x1000), KMALLOC - ((uintptr_t) &end + 0x1000));

		// Free unused (by BIOS) lower memory
		mem_free(KSPACE + 0x1000, 0x7F000);

		printk("%d KB used", pool_query(fpool) * 4);

	cursek(74, -1);
	printk("[done]");
}
