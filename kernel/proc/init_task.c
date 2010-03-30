/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <task.h>
#include <init.h>
#include <elf.h>

void init_task() {
	extern uint32_t get_eflags(void);
	process_t *init;
	uint32_t i, entry;
	void *user_init;

	/* Check for init */
	if (!mboot->mods_count) panic("No init found!");
	user_init = (void*) (*(uint32_t*) (mboot->mods_addr + KSPACE) + KSPACE);

	/* Check init */
	if (elf_check(user_init)) panic("init is not valid ELF");

	/* allocate stack space */
	init = process_get(1);
	for (i = USTACK_BSE; i < USTACK_TOP; i += PAGESZ) {
		page_set(i, page_fmt(frame_new(), PF_USER | PF_RW | PF_PRES));
	}

	/* Set TIS top pointer */
	init->image = thread_alloc();
	init->image->proc = init;

	/* Load executable */
	entry = elf_load(user_init);

	/* Setup process image */
	init->image->proc    = init;
	init->image->useresp = USTACK_INI;
	init->image->esp     = (uint32_t) &init->image->num;
	init->image->ebp     = USTACK_INI;
	init->image->ss      = 0x23;
	init->image->ds      = 0x23;
	init->image->eip     = entry;
	init->image->cs      = 0x1B;
	init->image->eflags  = get_eflags() | 0x3200; 	/* IF, IOPL=3 */
	tss_set_esp((uintptr_t) &init->image->tss_start);
}
