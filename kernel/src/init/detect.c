// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>

__attribute__ ((section(".ttext"))) 
void init_detect() {
	u32int i;
	printk("Detected: memory: ");

		// This is quite lazy - it finds the free block directly above 0x100000
		// Only a prototype - *please refactor*
		struct memory_map *mem_map = (void*) mboot->mmap_addr;
		u32int nmem_map = mboot->mmap_length / sizeof(struct memory_map);
		for (i = 0; i < nmem_map; i++) {
			if (mem_map[i].base_addr_low == 0x100000) {
				memsize = mem_map[i].length_low + 0x100000;
				printk("%d KB", (memsize - 0x100000) >> 10);
				break;
			}
		}

	cursek(74, -1);
	printk("[done]");
	printk("Detected: initrd: ");

		init_kload();
	
	cursek(74, -1);
	printk("[done]");
}
