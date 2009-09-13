/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <init.h>

__attribute__ ((section(".itext"))) 
void init_detect() {
	uint32_t i, nmem_map;
	struct memory_map *mem_map;

	printk("Detected: memory: ");

		/* This is quite lazy - it finds the free block directly above 0x100000 */
		/* Only a prototype - *please refactor* */
		mem_map = (void*) (mboot->mmap_addr + 0xFE000000);
		nmem_map = mboot->mmap_length / sizeof(struct memory_map);
		for (i = 0; i < nmem_map; i++) {
			if (mem_map[i].base_addr_low == 0x100000) {
				memsize = mem_map[i].length_low + 0x100000;
				if (memsize < 0x400000) memsize = 0x400000;
				printk("%d KB", memsize >> 10);
				break;
			}
		}

	cursek(74, -1);
	printk("[done]");	
}
