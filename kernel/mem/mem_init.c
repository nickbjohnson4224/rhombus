/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <mem.h>
#include <init.h>

/****************************************************************************
 * mem_init
 *
 * Initializes memory subsystem.
 */

void mem_init() {
	uintptr_t memsize;
	uintptr_t nmem_map;
	struct memory_map *mem_map;
	size_t i;

	/* detect physical memory size */
	mem_map = (void*) (mboot->mmap_addr + KSPACE);
	nmem_map = mboot->mmap_length / sizeof(struct memory_map);

	for (i = 0; i < nmem_map; i++) {
		if (mem_map[i].base_addr_low == 0x100000) {
			memsize = mem_map[i].length_low + 0x100000;
			if (memsize < 0x400000) memsize = 0x400000;
			break;
		}
	}

	/* initialize frame allocator */
	frame_init(memsize);

	/* touch pages for all kernel tables */
	for (i = KSPACE; i < KSPACE + SEGSZ * 6; i += SEGSZ) {
		page_touch(i);
	}

}
