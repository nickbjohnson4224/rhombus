// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <init.h>

// Assume at least 4MB of memory, because we can (it's a 386+)
u32int memsize = 0x400000;
u32int proto_base = 0xFF400000;

__attribute__ ((section(".ttext"))) 
void init_mem() {
	u32int i, *temp, cr3, initrd_end;
	printk("  Kernel: allocators");

		// Initialize frame allocator
		pool_new(memsize >> 12, fpool);

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: memory map");

		// Make a new and proper memory map for the kernel in the init map
		extern u32int init_kmap;
		u32int *kmap = &init_kmap;
		temp = (void*) 0xFF000000;
		pgclr(temp);
		temp[1023] = 0 | (PF_PRES | PF_RW);
		kmap[1022] = 0 | (PF_PRES | PF_RW);

		// Identity map necessary kernel memory (i.e. code and initrd)
		extern u32int init_ktbl;
		tmap[0xFF000000 >> 22] = ((u32int) &init_ktbl - 0xFF000000) | (PF_PRES | PF_RW);
		initrd_end = *(u32int*) (mboot->mods_addr + 0xFF000004) + 0xFF000000;
		for (i = 0xFF000000; i < initrd_end; i += 0x1000)
			ttbl[i >> 12] = frame_new() | (PF_PRES | PF_RW);

		// Reload the new map
		map_load(0);

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
		for (; i < 0xFF400000; i += 0x1000) {
			if (page_get(i) & PF_PRES) {
				frame_free(page_ufmt(page_get(i)));
				freed++;
			}
		}
		
		printk("%d KB freed", freed * 4);

	cursek(74, -1);
	printk("[done]");
}
