/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <time.h>
#include <space.h>
#include <init.h>

/****************************************************************************
 * thread_alloc
 *
 * Returns a pointer to a thread that was not previously allocated. Returns
 * null on error. The thread structure is page aligned.
 */

thread_t *thread_alloc(void) {
	return heap_alloc(sizeof(thread_t));
}

/****************************************************************************
 * thread_exit
 *
 * Kills the given thread and switches to another runnable thread.
 */

thread_t *thread_exit(thread_t *image) {
	thread_t *old_image;

	old_image = image;
	schedule_remove(old_image);

	image = thread_switch(image, schedule_next());

	thread_free(old_image);

	return image;
}

/****************************************************************************
 * thread_send
 *
 * Sends a signal to the process with pid targ of the type sig with the
 * granted page at current virtual address grant. If the target process cannot
 * accept it or has value SIG_POLICY_QUEUE at the signals' offset in 
 * signal_policy, the signal is added to that process' mailbox. Otherwise, a 
 * new thread is created in the target process to handle the incoming signal, 
 * and that thread is made active.
 *
 * The granted page in the current process is replaced with a page with
 * undefined contents and the same permissions as the granted page.
 *
 * Returns a runnable and active thread that may or may not be the thread
 * passed as image.
 */

thread_t *thread_send(thread_t *image, uint16_t targ, uint16_t sig) {
	extern void set_ts(void);
	process_t *p_targ;
	thread_t *new_image;

	p_targ = process_get(targ);

	if (image) {
		image->packet->signal = sig;
		image->packet->source = image->proc->pid;

		if (p_targ->port[sig].out) {
			p_targ->port[sig].in->next = image->packet;
			p_targ->port[sig].in = image->packet;
		}
		else {
			p_targ->port[sig].out = image->packet;
			p_targ->port[sig].in  = image->packet;
		}
	}

	if (p_targ->port[sig].entry) {
		new_image = thread_alloc();
		thread_bind(new_image, p_targ);

		new_image->ds      = 0x23;
		new_image->cs      = 0x1B;
		new_image->ss      = 0x23;
		new_image->eflags  = p_targ->thread[0]->eflags | 0x3000;
		new_image->useresp = new_image->stack + SEGSZ;
		new_image->proc    = p_targ;
		new_image->esi     = (image) ? image->proc->pid : 0;
		new_image->edi     = sig;
		new_image->eip     = p_targ->port[sig].entry;
		new_image->fxdata  = NULL;
		schedule_insert(new_image);

		if (image) {
			return thread_switch(image, new_image);
		}
		else {
			process_switch(new_image->proc);
			set_ts();
			return new_image;
		}
	}
	else {
		return image;
	}
}

/****************************************************************************
 * thread_free
 *
 * Frees a thread and its associated memory.
 */

void thread_free(thread_t *thread) {
	uintptr_t i;

	schedule_remove(thread);

	if (thread->stack) {
		i = (thread->stack - SSPACE) / SEGSZ;

		thread->proc->thread[i] = NULL;

		space_exmap(thread->proc->space);
	
		for (i = thread->stack; i < thread->stack + SEGSZ; i += PAGESZ) {
			if ((page_exget(i) & PF_PRES) != 0) {
				frame_free(page_ufmt(page_exget(i)));
				page_exset(i, 0);
			}
		}
	}

	heap_free(thread, sizeof(thread_t));
}

/****************************************************************************
 * thread_init
 *
 * Initializes threading subsystem.
 */

void thread_init(void) {
	uint16_t divisor;

	/* set up interrupt descriptor table */
	init_idt();

	/* initialize task state segment */
	init_tss();

	/* set programmable interrupt controller mask */
	pic_mask(0x0001);

	/* register system calls */
	register_int(0x40, syscall_send);
	register_int(0x41, syscall_done);
	register_int(0x42, syscall_when);
	register_int(0x43, syscall_recv);
	register_int(0x44, syscall_gvpr);
	register_int(0x45, syscall_svpr);

	register_int(0x48, syscall_fork);
	register_int(0x49, syscall_exit);
	register_int(0x4A, syscall_pctl);
	register_int(0x4B, syscall_exec);
	register_int(0x4C, syscall_gpid);
	register_int(0x4D, syscall_time);

	register_int(0x50, syscall_mmap);
	register_int(0x51, syscall_mctl);

	/* register fault handlers */
	register_int(0x00, fault_float);
	register_int(0x01, fault_generic);
	register_int(0x02, fault_generic);
	register_int(0x03, fault_generic);
	register_int(0x04, fault_generic);
	register_int(0x05, fault_generic);
	register_int(0x06, fault_generic);
	register_int(0x07, fault_nomath);
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
	register_int(0x13, fault_nomath);

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
 * thread_bind
 *
 * Returns a free segment of size SEGSZ with 16 pages allocated at its end.
 * Associates the thread with this stack with the process.
 */

uintptr_t thread_bind(thread_t *thread, process_t *proc) {
	uintptr_t i;
	uintptr_t addr;

	for (addr = 0, i = 0; i < 256; i++) {
		if (!proc->thread[i]) {
			proc->thread[i] = thread;
			addr = SSPACE + (SEGSZ * i);
			break;
		}
	}

	thread->stack = addr;
	thread->proc  = proc;

	return addr;
}

/****************************************************************************
 * thread_switch
 *
 * Switches to the target thread, completely saving the old threads' state
 * and loading the new ones' state. A process switch is performed if and only
 * if the threads are under different processes. A pointer to the switched to
 * thread is returned. If the new thread is null, the kernel idles until the
 * next thread switch attempt.
 */

thread_t *thread_switch(thread_t *old, thread_t *new) {
	extern void fpu_save(void *fxdata);
	extern bool tst_ts(void);
	extern void set_ts(void);
	extern void idle(void);

	if (!new) {
		idle();
	}

	/* save FPU state */
	if (old && !tst_ts()) {
		if (!old->fxdata) {
			old->fxdata = heap_alloc(512);
		}

		fpu_save(old->fxdata);
	}

	/* switch processes */
	if (!old || (old->proc != new->proc)) {
		process_switch(new->proc);
	}

	/* set task switched flag */
	set_ts();

	return new;
}
