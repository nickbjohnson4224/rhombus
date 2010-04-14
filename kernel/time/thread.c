/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>
#include <int.h>
#include <init.h>

/****************************************************************************
 * thread_alloc
 *
 * Returns a pointer to a thread that was not previously allocated. Returns
 * null on error. The thread structure is page aligned.
 */

thread_t *thread_alloc(void) {
	uintptr_t addr;
	thread_t *thread;
	size_t i;

	for (i = 0; i < 4096; i++) {
		addr = THREAD_TABLE + (PAGESZ * i);

		if ((page_get(addr) & PF_PRES) == 0) {
			page_set(addr, page_fmt(frame_new(), PF_PRES | PF_RW));

			thread = (thread_t*) addr;

			return thread;
		}
	}

	return NULL;
}

/****************************************************************************
 * thread_drop
 *
 * Kills the given thread and switches to another runnable thread. If legacy
 * mode is enabled, the other runnable thread is the next thread on the 
 * thread image stack of the current process.
 */

thread_t *thread_drop(thread_t *image) {
	thread_t *old_image;

	old_image = image;
	image->proc->image = image->tis;
	image = thread_switch(image, image->tis);
	thread_free(old_image);

	return image;
}

/****************************************************************************
 * thread_fire
 *
 * Sends a signal to the process with pid targ of the type sig with the
 * granted page at current virtual address grant. If the target process has 
 * the CTRL_QUEUE flag set, the signal is added to that process' mailbox. 
 * Otherwise, a new thread is created in the target process to handle the 
 * incoming signal, and that thread is switched to.
 *
 * The granted page in the current process is replaced with a page with
 * undefined contents and the same permissions as the granted page.
 *
 * Returns a runnable and active thread that may or may not be the thread
 * passed as image.
 */

thread_t *thread_fire(thread_t *image, uint16_t targ, uint16_t sig, uintptr_t grant){
	process_t *p_targ;
	thread_t *new_image;
	uintptr_t addr;

	p_targ = process_get(targ);

	if (grant) {
		addr   = grant;
		grant  = page_get(grant);
		page_set(addr, page_fmt(frame_new(), (grant & 0xFFF) | PF_PRES));
		grant &= ~0xFFF;
	}


	if (p_targ->flags & CTRL_QUEUE) {
		/* queue signal */

		/* XXX - please implement */
		return image;

	}
	else {
		/* send signal */

		if (p_targ->shandler == 0) {
			return image;
		}

		new_image = thread_alloc();

		new_image->ds      = 0x23;
		new_image->cs      = 0x1B;
		new_image->ss      = 0x23;
		new_image->eflags  = p_targ->image->eflags;
		new_image->useresp = p_targ->image->useresp;
		new_image->tis     = p_targ->image;
		new_image->proc    = p_targ;
		new_image->grant   = grant;
		new_image->ebx     = grant;
		new_image->source  = image->proc->pid;
		new_image->esi     = image->proc->pid;
		new_image->signal  = sig;
		new_image->edi     = sig;
		new_image->eip     = p_targ->shandler;

		p_targ->image  = new_image;
		p_targ->flags &= ~CTRL_BLOCK;
	
		return thread_switch(image, new_image);
	}
}

/****************************************************************************
 * thread_free
 *
 * Frees a thread and its associated memory.
 */

void thread_free(thread_t *thread) {
	frame_free(page_ufmt(page_get((uintptr_t) thread)));
	page_set((uintptr_t) thread, 0);
}

/****************************************************************************
 * thread_init
 *
 * Initializes threading subsystem.
 */

void thread_init(void) {
	uint16_t divisor;
	thread_t *image;

	/* set up interrupt descriptor table */
	init_idt();

	/* initialize task state segment */
	init_tss();

	/* set programmable interrupt controller mask */
	pic_mask(0x0001);

	/* register system calls */
	register_int(0x60, fire);
	register_int(0x61, drop);
	register_int(0x62, hand);
	register_int(0x63, ctrl);
	register_int(0x64, info);
	register_int(0x65, mmap);
	register_int(0x66, fork);
	register_int(0x67, exit);

	/* register fault handlers */
	register_int(0x00, fault_float);
	register_int(0x01, fault_generic);
	register_int(0x02, fault_generic);
	register_int(0x03, fault_generic);
	register_int(0x04, fault_generic);
	register_int(0x05, fault_generic);
	register_int(0x06, fault_generic);
	register_int(0x07, fault_float);
	register_int(0x08, fault_double);
	register_int(0x09, fault_float);
	register_int(0x0A, fault_generic);
	register_int(0x0B, fault_generic);
	register_int(0x0C, fault_generic);
	register_int(0x0D, fault_generic);
	register_int(0x0E, fault_page);
	register_int(0x0F, fault_generic);
	register_int(0x10, fault_float);
	register_int(0x11, fault_generic);
	register_int(0x12, fault_generic);

	/* initialize programmable interrupt timer at 256Hz */
	register_int(IRQ(0), pit_handler);
	divisor = 1193180 / 256;
	outb(0x43, 0x36);
	outb(0x40, (uint8_t) (divisor & 0xFF));
	outb(0x40, (uint8_t) (divisor >> 8));

	/* initialize FPU/MMX/SSE */
	init_fpu();
}

/****************************************************************************
 * thread_switch
 *
 * Switches to the target thread, completely saving the old threads' state
 * and loading the new ones' state. A process switch is performed if and only
 * if the threads are under different processes. A pointer to the switched to
 * thread is returned.
 */

thread_t *thread_switch(thread_t *old, thread_t *new) {
	extern void fpu_save(void *fxdata);
	extern void fpu_load(void *fxdata);

	/* save FPU state */
	if (old->proc->flags & CTRL_FLOAT) {
		fpu_save(old->fxdata);
	}

	/* switch processes */
	if (old->proc != new->proc) {
		process_switch(new->proc, 0);
	}

	/* load FPU state */
	if (new->proc->flags & CTRL_FLOAT) {
		fpu_load(new->fxdata);
	}

	return new;
}
