/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <interrupt.h>
#include <syscall.h>
#include <process.h>
#include <thread.h>
#include <string.h>
#include <fault.h>
#include <space.h>
#include <debug.h>
#include <timer.h>
#include <cpu.h>
#include <elf.h>

/* multiboot structures *****************************************************/

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

struct module {
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t string;
	uint32_t reserved;
} __attribute__ ((packed));

struct memory_map {
	uint32_t size;
	uint32_t base_addr_low;
	uint32_t base_addr_high;
	uint32_t length_low;
	uint32_t length_high;
	uint32_t type;
} __attribute__ ((packed));

/*****************************************************************************
 * init
 *
 * Initializes the kernel completely. Really, what did you expect it to do?
 * Returns the thread to "resume" (i.e. start from).
 *
 * This function is only called from "boot.s". It's not declared in any header
 * files, but don't make it static.
 */

struct thread *init(struct multiboot *mboot, uint32_t mboot_magic) {
	struct process *idle, *init;
	struct module *module;
	struct memory_map *mem_map;
	size_t mem_map_count, i, addr;
	uintptr_t boot_image_size;
	void *boot_image;
	struct elf32_ehdr *init_image;

	/* initialize debugging output */
	debug_init();
	debug_printf("Flux Operating System Kernel v0.7a\n");

	/* check multiboot header */
	if (mboot_magic != 0x2BADB002) {
		debug_panic("bootloader is not multiboot compliant");
	}

	/* touch pages for the kernel heap */
	for (i = KSPACE; i < KERNEL_HEAP_END; i += SEGSZ) {
		page_touch(i);
	}

	/* parse the multiboot memory map to find the size of memory */
	mem_map       = (void*) (mboot->mmap_addr + KSPACE);
	mem_map_count = mboot->mmap_length / sizeof(struct memory_map);

	for (i = 0; i < mem_map_count; i++) {
		if (mem_map[i].type == 1) {
			for (addr = 0; addr < mem_map[i].length_low; addr += PAGESZ) {
				frame_add(mem_map[i].base_addr_low + addr);
			}
		}
	}

	/* bootstrap process 0 (idle) */
	idle = process_alloc();
	idle->space = cpu_get_cr3();
	idle->user  = 0;

	/* fork process 1 (init) and switch */
	init = process_clone(idle, NULL);
	process_switch(init);

	/* get multiboot module information */
	if (mboot->mods_count < 2) {
		if (mboot->mods_count < 1) {
			debug_panic("no init or boot modules found");
		}
		else {
			debug_panic("no boot module found");
		}
	}
	module     = (void*) (mboot->mods_addr + KSPACE);
	init_image = (void*) (module[0].mod_start + KSPACE);
	boot_image = (void*) (module[1].mod_start + KSPACE);
	boot_image_size = module[1].mod_end - module[1].mod_start;

	/* move boot image to BOOT_IMAGE in userspace */
	mem_alloc(BOOT_IMAGE, boot_image_size, PF_PRES | PF_USER | PF_RW);
	memcpy((void*) BOOT_IMAGE, boot_image, boot_image_size);

	/* bootstrap thread 0 in init */
	thread_bind(init->thread[0], init);
	init->thread[0]->useresp = init->thread[0]->stack + SEGSZ;
	init->thread[0]->esp     = (uintptr_t) &init->thread[0]->num;
	init->thread[0]->ss      = 0x23;
	init->thread[0]->ds      = 0x23;
	init->thread[0]->cs      = 0x1B;
	init->thread[0]->eflags  = cpu_get_eflags() | 0x3200; /* IF, IOPL = 3 */

	/* execute init */
	if (elf_check_file(init_image)) {
		debug_panic("init is not a valid ELF executable");
	}
	elf_load_file(init_image);
	init->thread[0]->eip = init_image->e_entry;

	/* register system calls */
	int_set_handler(SYSCALL_SEND, syscall_send);
	int_set_handler(SYSCALL_DONE, syscall_done);
	int_set_handler(SYSCALL_WHEN, syscall_when);
	int_set_handler(SYSCALL_RIRQ, syscall_rirq);
	int_set_handler(SYSCALL_ALSO, syscall_also);
	int_set_handler(SYSCALL_STAT, syscall_stat);
	int_set_handler(SYSCALL_PAGE, syscall_page);
	int_set_handler(SYSCALL_PHYS, syscall_phys);
	int_set_handler(SYSCALL_FORK, syscall_fork);
	int_set_handler(SYSCALL_EXIT, syscall_exit);
	int_set_handler(SYSCALL_STOP, syscall_stop);
	int_set_handler(SYSCALL_WAKE, syscall_wake);
	int_set_handler(SYSCALL_GPID, syscall_gpid);
	int_set_handler(SYSCALL_TIME, syscall_time);
	int_set_handler(SYSCALL_USER, syscall_user);
	int_set_handler(SYSCALL_AUTH, syscall_auth);
	int_set_handler(SYSCALL_PGRP, syscall_pgrp);
	int_set_handler(SYSCALL_KILL, syscall_kill);

	/* register fault handlers */
	int_set_handler(FAULT_DE, fault_float);
	int_set_handler(FAULT_DB, fault_generic);
	int_set_handler(FAULT_NI, fault_generic);
	int_set_handler(FAULT_BP, fault_generic);
	int_set_handler(FAULT_OF, fault_generic);
	int_set_handler(FAULT_BR, fault_generic);
	int_set_handler(FAULT_UD, fault_generic);
	int_set_handler(FAULT_NM, fault_nomath);
	int_set_handler(FAULT_DF, fault_double);
	int_set_handler(FAULT_CO, fault_float);
	int_set_handler(FAULT_TS, fault_generic);
	int_set_handler(FAULT_NP, fault_generic);
	int_set_handler(FAULT_SS, fault_generic);
	int_set_handler(FAULT_GP, fault_generic);
	int_set_handler(FAULT_PF, fault_page);
	int_set_handler(FAULT_MF, fault_float);
	int_set_handler(FAULT_AC, fault_generic);
	int_set_handler(FAULT_MC, fault_generic);
	int_set_handler(FAULT_XM, fault_nomath);

	/* start timer (for preemption) */
	timer_set_freq(256);

	/* initialize FPU/MMX/SSE */
	cpu_init_fpu();

	/* drop to usermode, scheduling the next thread */
	debug_printf("dropping to usermode\n");
	return thread_switch(NULL, schedule_next());
}
