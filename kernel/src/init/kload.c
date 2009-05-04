// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>

__attribute__ ((section(".tdata")))
struct tar_header *header[256];

__attribute__ ((section(".tdata")))
struct tar_header *initrd;

__attribute__ ((section(".ttext")))
static int tar_header_check(struct tar_header *t) {
	u32int i, sum = 0;
	u8int *header_byte = (u8int*) t;
	for (i = 0;   i < 512; i++) sum += header_byte[i];
	for (i = 148; i < 156; i++) sum -= header_byte[i]; 	// Discount checksum itself
	for (i = 148; i < 156; i++) sum += (u8int) ' ';		// Count checksum as spaces
	if (atoi(t->chksum, 8) == sum) return sum;
	return -1;
}

__attribute__ ((section(".ttext")))
static char * header_contents(struct tar_header *t) {
	return (char*) ((u32int) t + sizeof(struct tar_header));
}

__attribute__ ((section(".ttext")))
void init_kload() {
	// Check for initrd (it's really a tape archive)
	if (!mboot->mods_count) panic("No libsys/driver file found!");
	initrd = (void*) *(u32int*) (mboot->mods_addr + 0xF8000000) + 0xF8000000;
	u32int size = ((*(u32int*) (mboot->mods_addr + 0xF8000004) + 0xF8000000) - (u32int) initrd) / 512;
	printk("%d blocks", size);

	// Check validity of tarball
	if (tar_header_check(initrd) == -1) panic("Tar checksum error");

	// Index initrd for later use
	u32int i, n;
	for (i = 0, n = 0; i < size; i++) if (tar_header_check(&initrd[i]) != -1)
		header[n++] = &initrd[i];
	header[n] = NULL;
}

__attribute__ ((section(".ttext")))
void init_libsys() {

	// Check for libsys header
	u8int n;
	for (n = 0; n < 256; n++) if (!strcmp(header[n]->name, "libsys")) break;
	if (n == 256) panic("No system library found");

}
