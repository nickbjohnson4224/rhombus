/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <init.h>
#include <mem.h>

uint32_t memsize;

__attribute__ ((section(".itext"))) 
void init_mem() {
	uint32_t i, z;

	printk("  Kernel: allocators");

		/* Initialize frame allocator */
		fpool = pool_new(memsize >> 12);

		/* Set the first 4 MB to used (appropriately) */
		for (i = 0; i < 1024; i++) z = frame_new();

	cursek(74, -1);
	printk("[done]");
}

void init_free() {
	extern uint32_t START_OF_INIT;
	extern uint32_t END_OF_INIT;
	extern uint32_t end;
	uint32_t base, limit;

	printk("  Kernel: GC: ");

		/* Free marked code and global data */
		base = (uint32_t) &START_OF_INIT;
		limit = (uint32_t) &END_OF_INIT;
		mem_free(base, limit - base);

		/* Free initrd image data */
		mem_free(((uintptr_t) &end + 0x1000), KMALLOC - ((uintptr_t) &end + 0x1000));

		/* Free unused (by BIOS) lower memory */
		mem_free(KSPACE + 0x1000, 0x7F000);

		printk("%d KB used", pool_query(fpool) * 4);

	cursek(74, -1);
	printk("[done]");
}
