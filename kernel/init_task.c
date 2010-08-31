/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <string.h>
#include <debug.h>
#include <util.h>
#include <time.h>
#include <init.h>
#include <elf.h>

void init_task() {
	extern uint32_t get_eflags(void);
	struct process *init;
	struct module *module;
	uintptr_t boot_size;
	void *init_image;
	void *boot_image;

	init = process_get(1);

	module     = (void*) (mboot->mods_addr + KSPACE);
	init_image = (void*) (module[0].mod_start + KSPACE);
	boot_image = (void*) (module[1].mod_start + KSPACE);
	boot_size  = module[1].mod_end - module[1].mod_start;

	/* Check init */
	if (elf_check(init_image)) {
		debug_panic("Init is not valid ELF");
	}

	/* Map boot image */
	mem_alloc(BOOT_IMAGE, boot_size, PF_PRES | PF_USER | PF_RW);
	memcpy((void*) BOOT_IMAGE, boot_image, boot_size);

	/* Setup process image */
	thread_bind(init->thread[0], init);
	init->thread[0]->useresp = init->thread[0]->stack + SEGSZ;
	init->thread[0]->esp     = (uint32_t) &init->thread[0]->num;
	init->thread[0]->ss      = 0x23;
	init->thread[0]->ds      = 0x23;
	init->thread[0]->eip     = elf_load(init_image);
	init->thread[0]->cs      = 0x1B;
	init->thread[0]->eflags  = get_eflags() | 0x3200; 	/* IF, IOPL=3 */
	tss_set_esp((uintptr_t) &init->thread[0]->tss_start);
}
