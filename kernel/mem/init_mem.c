/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <init.h>
#include <mem.h>

extern uint32_t memsize;

void init_mem() {
	uint32_t i, z;

	printk("  Kernel: frame allocator");

		frame_init();

	cursek(74, -1);
	printk("[done]");
}

void init_free() {
	extern uint32_t end;
	extern uint32_t allocator_space; 
	uint32_t base, limit;

	printk("  Kernel: GC: ");

		/* Free initrd image data */
		mem_free(((uintptr_t) &end + PAGESZ), 
			KSPACE + SEGSZ - ((uintptr_t) &end + PAGESZ));

		/* Free unused (by BIOS) lower memory (0x1000 - 0x80000) */
		mem_free(KSPACE + 0x1000, 0x7F000);

	cursek(74, -1);
	printk("[done]");
}
