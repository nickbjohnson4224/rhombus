/* Copyright 2009, 2010 Nick Johnson */

#include <config.h>
#include <lib.h>
#include <init.h>

#ifdef KERNEL_GC
__attribute__ ((section(".itext")))
#endif
void init_detect() {
	uintptr_t nmem_map;
	size_t i;
	struct memory_map *mem_map;

	printk("Detected: memory: ");

		/* This is quite lazy - it finds the free block directly above 0x100000 */
		/* Fix for systems with > 1GB of RAM to get past ACPI mmapped space */
		mem_map = (void*) (mboot->mmap_addr + KSPACE);
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
