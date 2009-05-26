// Copyright 2009 Nick Johnson

#include <lib.h>
#include <mem.h>
#include <trap.h>
#include <task.h>
#include <init.h>

__attribute__ ((section(".tdata"))) 
char *stamp = "\
Khaos - 0.0 build 013\n\
Copyright 2009 Nick Johnson\n\n";

typedef void (*init_t)(void);
__attribute__ ((section(".tdata")))
init_t init_list[] = {
init_detect,
init_mem,
init_int,
init_task,
init_pit,
init_free,
NULL
};

void init(void *mboot_ptr, u32int mboot_magic) {
	u32int i;

	cleark();
	printk(stamp);

	if (mboot_magic != 0x2BADB002) panic("Bootloader is not multiboot compliant");
	else mboot = mboot_ptr;

	printk("Booting Khaos\n");
	for (i = 0; init_list[i]; i++) init_list[i]();
	printk("Khaos booted\n\n");
}
