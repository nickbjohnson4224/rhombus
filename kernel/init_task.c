/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <init.h>
#include <elf.h>

__attribute__ ((section(".itext"))) 
void init_task() {
	task_t *idle;

	printk("  Kernel: tasking system");

		/* Set up task table and allocator */
		tpool = pool_new(MAX_TASKS);

		/* Bootstrap task 0, because there is nothing to fork from */
		task_touch(0);
		idle = task_get(0);
		idle->pid = (pid_t) pool_alloc(tpool);
		idle->map = map_clone();
		idle->flags = TF_READY | TF_SUPER;
		tss_set_esp(SSTACK_INI);

		/* Make sure the scheduler works */
		queue.next = 0;
		queue.last = 0;

		/* Fork task 1 */
		task_switch(task_new(task_get(0)));

		/* Mark task 0 as gone */
		idle->magic = 0;

	cursek(74, -1);
	printk("[done]");
	printk("  Kernel: loading init");

		/* Load init from the initrd */
		init_user_init();

	cursek(74, -1);
	printk("[done]");
}

__attribute__ ((section(".itext")))
void init_user_init() {
	extern uint32_t get_eflags(void);
	uint32_t i;
	void *user_init;
	task_t *t;

	/* Check for user init */
	if (!mboot->mods_count) panic("No init found!");
	user_init = (void*) (*(uint32_t*) (mboot->mods_addr + KSPACE) + KSPACE);

	/* Set up a stack for the process image */
	t = task_get(curr_pid);

	for (i = USTACK_BSE; i < USTACK_TOP; i += 0x1000) p_alloc(i, (PF_USER | PF_RW));
	for (i = SSTACK_BSE; i < SSTACK_TOP; i += 0x1000) p_alloc(i, (PF_USER | PF_RW));
	t->image = (void*) (SSTACK_INI - sizeof(image_t));

	/* Set up space for the signal handler table */
	p_alloc(SIG_TBL, (PF_USER | PF_RW));
	pgclr((void*) SIG_TBL);

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
}
