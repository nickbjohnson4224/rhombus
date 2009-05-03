// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <init.h>

// Assume at least 4MB of memory, because we can, and it won't boot otherwise
u32int memsize = 0x400000;

__attribute__ ((section(".ttext"))) 
void init_mem() {
	u32int i;

	colork(0xC);

	// Make a new and proper memory map for the kernel
	map_alloc(&kmap);

	printk("Frame allocator");
	fpool = pool_new(memsize >> 12);
	cursek(36, -1);
	printk("done\n");

	printk("Kernel heap allocator");

	// Preallocate page tables for the allocator, to prevent infinte recursion
	i = 0xF8400000;
	while (i) {
		page_touch(&kmap, i);
		i += 0x400000;
	}

	// Create the allocator proper, with enough space to address all of kernelspace
	ppool = pool_new(0x07C00000 >> 12);

	cursek(36, -1);
	printk("done\n");

	printk("Kernel address space");

	// Identity map necessary kernel memory (i.e. code)
	i = 0xF8000000;
	extern u32int end;
	while (i < (u32int) 0xF8400000) {
		page_touch(&kmap, i);
		page_t new_page = page_fmt(frame_new(), PF_PRES | PF_RW);
		page_set(&kmap, i, new_page);
		i += 0x1000;
	}
	cursek(36, -1);
	printk("done\n");


	printk("Entering full paging");

	// Link the kernel to lower memory temporarily
	kmap.pdir[0] = kmap.pdir[0xF8000000 >> 22];

	// Load the new memory map from lower memory
	redo_paging(phys_of(&kmap, kmap.pdir));

	// Clear the unneeded linking for safety
	kmap.pdir[0] = 0x00000000;

	// Mark memory as set up, so the new allocator is used
	mem_setup = 1;

	cursek(36, -1);
	printk("done\n");

	colork(0xF);

}

void init_free() {
	colork(0xC);
	printk("Freeing temporary memory");

	extern u32int START_OF_TEMP;
	u32int base = (u32int) &START_OF_TEMP;

	extern u32int END_OF_TEMP;
	u32int limit = (u32int) &END_OF_TEMP;

	u32int i;
	for (i = base; i < limit; i += 0x1000) {
		frame_free(page_ufmt(page_get(&kmap, i)));
		page_set(&kmap, i, 0x00000000);
	}

	cursek(36, -1);
	printk("done\n");

	colork(0xF);
}
