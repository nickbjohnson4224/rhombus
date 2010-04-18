/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <util.h>
#include <time.h>
#include <init.h>
#include <elf.h>

void init_task() {
	extern uint32_t get_eflags(void);
	process_t *init;
	uint32_t entry;
	void *user_init;

	init = process_get(1);

	/* Check for init */
	if (!mboot->mods_count) panic("No init found!");
	user_init = (void*) (*(uint32_t*) (mboot->mods_addr + KSPACE) + KSPACE);

	/* Check init */
	if (elf_check(user_init)) panic("init is not valid ELF");

	/* Setup process image */
	init->thread[0]->proc    = init;
	init->thread[0]->stack   = thread_stack_alloc(init->thread[0], init);
	init->thread[0]->useresp = init->thread[0]->stack + SEGSZ;
	init->thread[0]->esp     = (uint32_t) &init->thread[0]->num;
	init->thread[0]->ss      = 0x23;
	init->thread[0]->ds      = 0x23;
	init->thread[0]->eip     = elf_load(user_init);
	init->thread[0]->cs      = 0x1B;
	init->thread[0]->eflags  = get_eflags() | 0x3200; 	/* IF, IOPL=3 */
	tss_set_esp((uintptr_t) &init->thread[0]->tss_start);
}
