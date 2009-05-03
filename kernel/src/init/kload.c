// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>

__attribute__ ((section(".tdata")))
struct tar_header *header[256];

__attribute__ ((section(".ttext")))
static u8int tar_header_check(struct tar_header *t) {
	return 0;
}

__attribute__ ((section(".ttext")))
void init_kload() {
	// Check for initrd (it's really a tape archive)
	if (!mboot->mods_count) panic("No libsys/driver file found!");

	// Index initrd for later use
	u32int *mod = (u32int*) (mboot->mods_addr + 0xF8000000);
	printk("\n%x: %x %x\n", mod, mod[0], mod[1]);
	for(;;);
}

void init_libsys();
	
