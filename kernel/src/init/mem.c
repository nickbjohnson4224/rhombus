// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <init.h>

// Assume at least 4MB of memory, because we can (it's a 386+)
u32int memsize = 0x400000;

__attribute__ ((section(".ttext"))) 
void init_mem() {
	u32int i;
	colork(0xC);
	printk("Allocators");

		// Make a new and proper memory map for the kernel
		map_alloc(&kmap);

		// Initialize frame allocator
		fpool = pool_new(memsize >> 12);

		// Create the address space allocator
		ppool = pool_new(0x07C00000 >> 12);	

		// Preallocate page tables for the address space allocator
		for (i = 0xF8400000; i; i += 0x400000) page_touch(&kmap, i);

	cursek(36, -1);
	printk("done\n");
	printk("Kernel map");

		// Identity map necessary kernel memory (i.e. code and initrd)
		i = 0xF8000000;
		extern u32int end;
		while (i < (u32int) 0xF8400000) {
			page_touch(&kmap, i);
			page_t new_page = page_fmt(frame_new(), PF_PRES | PF_RW);
			page_set(&kmap, i, new_page);
			i += 0x1000;
		}

		// Reload the new map by going through physical memory
		kmap.pdir[0] = kmap.pdir[0xF8000000 >> 22];
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
		for (i = base; i < limit & ~0xFFF; i += 0x1000) p_free(&kmap, i);

		// Free initrd image data
		extern u32int end;
		i = ((u32int) &end + 0x1000) & ~0xFFF;
		for (; i < 0xF8400000; i += 0x1000)
			if (page_get(&kmap, i)) frame_free(page_ufmt(page_get(&kmap, i)));

	cursek(36, -1);
	printk("done\n");
	colork(0xF);
}
