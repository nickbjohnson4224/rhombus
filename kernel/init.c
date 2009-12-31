/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <mem.h>
#include <int.h>
#include <task.h>
#include <init.h>

#define FLUX_VERSION_MAJOR 0
#define FLUX_VERSION_MINOR 2
#define FLUX_KERNEL_REVISN 1

__attribute__ ((section(".tdata"))) 
const char *stamp = "\
Flux Operating System v%d.%d\n\
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
init_fpu,
NULL
};

struct multiboot *mboot;

typedef void (*entry_t)();

void init(void *mboot_ptr, uint32_t mboot_magic) {
	uint32_t i;

	cleark(); 
	printk(stamp, FLUX_VERSION_MAJOR, FLUX_VERSION_MINOR);

	if (mboot_magic != 0x2BADB002) {
		panic("Bootloader is not multiboot compliant");
	}
	else mboot = mboot_ptr;

	for (i = 0; init_list[i]; i++) {
		init_list[i]();
	}

	printk("  Kernel: dropping to usermode");
	cursek(74, -1);
	printk("[done]");
}
