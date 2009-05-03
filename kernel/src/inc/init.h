// Copyright 2009 Nick Johnson

#ifndef INIT_H
#define INIT_H

// Multiboot header
struct multiboot {
	u32int flags;
	u32int mem_lower;
	u32int mem_upper;
	u32int boot_device;
	u32int cmdline;
	u32int mods_count;
	u32int mods_addr;
	u32int num;
	u32int size;
	u32int addr;
	u32int shndx;
	u32int mmap_length;
	u32int mmap_addr;
	u32int drives_length;
	u32int drives_addr;
	u32int config_table;
	u32int boot_loader_name;
	u32int apm_table;
	u32int vbe_control_info;
	u32int vbe_mode_info;
	u32int vbe_mode;
	u32int vbe_interface_seg;
	u32int vbe_interface_off;
	u32int vbe_interface_len;
}  __attribute__((packed));

struct memory_map {
	u32int size;
	u32int base_addr_low;
	u32int base_addr_high;
	u32int length_low;
	u32int length_high;
	u32int type;
} __attribute__ ((packed));

struct multiboot *mboot;

u32int memsize;

// Standard V7 TAR header
struct tar_header {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char link;
	char linkname[100];
	u8int padding[255];
} __attribute__ ((packed));

void init_detect();
void init_mem();
void init_int();

void init_kload();
void init_libsys();

#endif /*INIT_H*/
