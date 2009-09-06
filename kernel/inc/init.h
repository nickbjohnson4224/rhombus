/* Copyright 2009 Nick Johnson */

#ifndef INIT_H
#define INIT_H

#include <test.h>

/* Multiboot header */
struct multiboot {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
}  __attribute__((packed));

/* Multiboot memory map */
struct memory_map {
	uint32_t size;
	uint32_t base_addr_low;
	uint32_t base_addr_high;
	uint32_t length_low;
	uint32_t length_high;
	uint32_t type;
} __attribute__ ((packed));

extern struct multiboot *mboot;
extern uint32_t memsize;

/* Standard V7 TAR header */
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
	char padding[255];
} __attribute__ ((packed));

void init(void *mboot_ptr, uint32_t mboot_magic);
void init_detect(void);
void init_mem(void);
void init_free(void);
void init_int(void);
void init_kload(void);
void init_user_init(void);
void init_sysmap(void);
void init_task(void);
void init_fault(void);
void init_tss(void);
void init_idt(void);
void init_pit(void);
void init_initrd_rmap(void);

#endif /*INIT_H*/
