// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <init.h>

// Assume at least 4MB of memory, because we can (it's a 386+)
u32int memsize = 0x400000;
u32int proto_base = 0xFF400000;

__attribute__ ((section(".ttext"))) 
void init_mem() {
	u32int i;
	printk("  Kernel: allocators");

		// Initialize frame allocator
		pool_new(memsize >> 12, fpool);

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: memory map");

		// Make a new and proper memory map for the kernel in the init map
		extern u32int *init_kmap;
		init_kmap[1023] = frame_new() | (PF_PRES | PF_RW);

		// Identity map necessary kernel memory (i.e. code and initrd)
		for (i = 0xFF000000; i < 0xFF400000; i += 0x1000)
			page_set(i, frame_new() | (PF_PRES | PF_RW));

		// Reload the new map by going through physical memory
		extern void redo_paging(u32int);
		cmap[0] = cmap[0xFF000000 >> 22];
		redo_paging(init_kmap[1023]);
		cmap[0] = 0;

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
		for (i = base; i < (limit & ~0xFFF); i += 0x1000) {
			p_free(i);
			freed++;
		}
		
		// Free initrd image data
		extern u32int end;
		i = ((u32int) &end + 0x1000) & ~0xFFF;
		for (; i < 0xF8400000; i += 0x1000) {
			if (page_get(i)) {
				frame_free(page_ufmt(page_get(i)));
				freed++;
			}
		}
		
		printk("%d KB freed", freed * 4);

	cursek(74, -1);
	printk("[done]");
}
