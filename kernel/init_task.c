/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <lib.h>
#include <task.h>
#include <init.h>
#include <elf.h>

#ifdef KERNEL_GC
__attribute__ ((section(".itext")))
#endif
void init_task() {
	extern uint32_t get_eflags(void);
	extern uint32_t get_cr3(void);
	task_t *idle, *t;
	uint32_t i, entry;
	void *user_init;

	printk("  Kernel: tasking system");

		/* Set up task allocator */
		tpool = pool_new(MAX_TASKS);

		/* Bootstrap task 0 */
		task_touch(0);
		idle = task_get(0);
		idle->pid = 0;
		idle->map = get_cr3();
		idle->flags = CTRL_READY | CTRL_SUPER;
		idle->sigflags = CTRL_SENTER;

		task_switch(task_new(idle), 0); /* Fork task 1 and switch */
		idle->magic = 0;				/* Mark task 0 as invalid */

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: loading init");

		/* Check for init */
		if (!mboot->mods_count) panic("No init found!");
		user_init = (void*) (*(uint32_t*) (mboot->mods_addr + KSPACE) + KSPACE);
		
		/* Check init */
		if (elf_check(user_init)) panic("init is not valid ELF");

		/* Set up stacks for the new task - TIS and user stack */
		t = task_get(curr_pid);
		for (i = USTACK_BSE; i < USTACK_TOP; i += 0x1000) {
			p_alloc(i, (PF_USER | PF_RW));
		}
		for (i = SSTACK_BSE; i < SSTACK_TOP; i += 0x1000) {
			p_alloc(i, PF_USER);
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
