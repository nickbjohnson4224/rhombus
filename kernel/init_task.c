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
	extern uint32_t get_cr3(void);
	process_t *idle, *t;
	uint32_t i, entry;
	void *user_init;

	printk("  Kernel: tasking system");

		/* Bootstrap task 0 */
		idle = process_alloc();
		idle->space = get_cr3();
		idle->flags = CTRL_READY | CTRL_SUPER;
		idle->sigflags = CTRL_SENTER;

		process_switch(process_clone(idle), 0); 	/* Fork task 1 and switch */

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: loading init");

		/* Check for init */
		if (!mboot->mods_count) panic("No init found!");
		user_init = (void*) (*(uint32_t*) (mboot->mods_addr + KSPACE) + KSPACE);
		
		/* Check init */
		if (elf_check(user_init)) panic("init is not valid ELF");

		/* Set up stacks for the new task - TIS and user stack */
		t = process_get(curr_pid);
		for (i = USTACK_BSE; i < USTACK_TOP; i += PAGESZ) {
			page_set(i, page_fmt(frame_new(), PF_USER | PF_RW | PF_PRES));
		}
		
		for (i = SSTACK_BSE; i < SSTACK_TOP; i += PAGESZ) {
			page_set(i, page_fmt(frame_new(), PF_USER | PF_RW | PF_PRES));
		}

		/* Set TIS top pointer */
		t->image = (void*) (SSTACK_INI - sizeof(thread_t));

		/* Load executable */
		entry = elf_load(user_init);

		/* Setup process image */
		t->image->useresp = USTACK_INI;
		t->image->esp = (uint32_t) &t->image->num;
		t->image->ebp = USTACK_INI;
		t->image->ss  = 0x23;
		t->image->ds  = 0x23;
		t->image->eip = entry;
		t->image->cs  = 0x1B;
		t->image->eflags = get_eflags() | 0x3200; 	/* IF, IOPL=3 */
		tss_set_esp((uint32_t) &t->image->tss_start);

	cursek(74, -1);
	printk("[done]");
}
