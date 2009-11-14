/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <init.h>
#include <elf.h>

__attribute__ ((section(".itext"))) 
void init_task() {
	extern uint32_t get_eflags(void);
	extern uint32_t get_cr3(void);
	task_t *idle, *t;
	uint32_t i;
	void *user_init;

	printk("  Kernel: tasking system");

		tpool = pool_new(MAX_TASKS);		/* Set up task allocator */

		/* Bootstrap task 0 */
		task_touch(0);
		idle = task_get(0);
		idle->pid = (pid_t) pool_alloc(tpool);
		idle->map = get_cr3();
		idle->flags = TF_READY | TF_SUPER;

		task_switch(task_new(task_get(0))); /* Fork task 1 */
		idle->magic = 0;					/* Mark task 0 as gone */

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: loading init");

		/* Check for user init */
		if (!mboot->mods_count) panic("No init found!");
		user_init = (void*) (*(uint32_t*) (mboot->mods_addr + KSPACE) + KSPACE);

		/* Set up a stack for the process image */
		t = task_get(curr_pid);
		for (i = USTACK_BSE; i < USTACK_TOP; i += 0x1000) p_alloc(i, (PF_USER | PF_RW));
		for (i = SSTACK_BSE; i < SSTACK_TOP; i += 0x1000) p_alloc(i, (PF_USER | PF_RW));
		t->image = (void*) (SSTACK_INI - sizeof(image_t));

		/* Load libsys image */
		if (elf_check(user_init)) panic("init is not valid ELF");

		/* Setup process image */
		t->image->useresp = USTACK_INI;
		t->image->esp = USTACK_INI;
		t->image->ebp = USTACK_INI;
		t->image->ss = 0x23;
		t->image->ds = 0x23;
		t->image->eip = elf_load(user_init);
		t->image->cs = 0x1B;
		t->image->eflags = get_eflags() | 0x3200; /* Turns on interrupts, IOPL=3 in eflags */
		t->image->mg = 0x42242442;

	cursek(74, -1);
	printk("[done]");
}
