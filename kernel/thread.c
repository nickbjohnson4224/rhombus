/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <ktime.h>
#include <space.h>
#include <init.h>
#include <debug.h>

/****************************************************************************
 * thread_alloc
 *
 * Returns a pointer to a thread that was not previously allocated. Returns
 * null on error. The thread structure is page aligned.
 */

struct thread *thread_alloc(void) {
	return heap_alloc(sizeof(struct thread));
}

/****************************************************************************
 * thread_exit
 *
 * Kills the given thread and switches to another runnable thread.
 */

struct thread *thread_exit(struct thread *image) {
	struct thread *old_image;

	old_image = image;
	schedule_remove(old_image);

	image = thread_switch(image, schedule_next());

	thread_free(old_image);

	return image;
}

/****************************************************************************
 * thread_send
 *
 * Sends an event to the process with pid target and port port. A new thread 
 * is created in the target process to handle the incoming event, and that 
 * thread is made active.
 *
 * Returns a runnable and active thread that may or may not be the thread
 * passed as image.
 */

struct thread *thread_send(struct thread *image, uint16_t target, uint16_t port) {
	extern void set_ts(void);
	struct process *p_targ;
	struct thread *new_image;

	p_targ = process_get(target);
	new_image = thread_alloc();
	thread_bind(new_image, p_targ);
	schedule_insert(new_image);

	new_image->ds      = 0x23;
	new_image->cs      = 0x1B;
	new_image->ss      = 0x23;
	new_image->eflags  = p_targ->thread[0]->eflags | 0x3000;
	new_image->useresp = new_image->stack + SEGSZ;
	new_image->proc    = p_targ;
	new_image->eip     = 0xC000;
	new_image->fxdata  = NULL;

	if (image) {
		new_image->packet         = image->packet;
		new_image->packet->source = image->proc->pid;
		new_image->packet->port   = port;
	}
	else {
		new_image->packet         = heap_alloc(sizeof(struct packet));
		new_image->packet->port   = port;
	}

	return thread_switch(image, new_image);
}

/****************************************************************************
 * thread_free
 *
 * Frees a thread and its associated memory.
 */

void thread_free(struct thread *thread) {
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

	heap_free(thread, sizeof(struct thread));
}

/****************************************************************************
 * thread_init
 *
 * Initializes threading subsystem.
 */

void thread_init(void) {
	uint16_t divisor;
	extern uint32_t get_cr0(void);

	/* set up interrupt descriptor table */
	init_idt();

	/* initialize task state segment */
	init_tss();

	/* set programmable interrupt controller mask */
	pic_mask(0x0005);

	/* register system calls */
	register_int(0x40, syscall_send);
	register_int(0x41, syscall_done);

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
 * thread_freeze
 *
 * Prevents the given thread from running until it is later thawed. If the
 * thread is already frozen, the frozen count is incremented. Returns the 
 * given thread.
 */

struct thread *thread_freeze(struct thread *thread) {

	if (!thread->frozen) {
		schedule_remove(thread);
	}

	thread->frozen++;

	return thread;
}

/****************************************************************************
 * thread_thaw
 *
 * Allows the given thread to run if its frozen count is less than two.
 * Otherwise, the given thread's frozen count is decremented. Returns the
 * given thread.
 */

struct thread *thread_thaw(struct thread *thread) {
	
	if (thread->frozen) {
		thread->frozen--;
	}

	if (!thread->frozen) {
		schedule_insert(thread);
	}

	return thread;
}

/****************************************************************************
 * thread_bind
 *
 * Returns a free segment of size SEGSZ with 16 pages allocated at its end.
 * Associates the thread with this stack with the process.
 */

uintptr_t thread_bind(struct thread *thread, struct process *proc) {
	uintptr_t i;
	uintptr_t addr;

	for (addr = 0, i = 0; i < MAX_THREADS; i++) {
		if (!proc->thread[i]) {
			proc->thread[i] = thread;
			addr = SSPACE + (SEGSZ * i);
			break;
		}
	}

	thread->id    = i;
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

struct thread *thread_switch(struct thread *old, struct thread *new) {
	extern void fpu_save(void *fxdata);
	extern void fpu_load(void *fxdata);
	extern bool tst_ts(void);
	extern void set_ts(void);
	extern uint32_t get_cr0(void);
	extern void idle(void);

	if (!new) {
		idle();
	}

	/* save FPU state */
	if (old) {
		if (!old->fxdata) {
			old->fxdata = heap_alloc(512);
		}

		fpu_save(old->fxdata);
	}
	
	/* switch processes */
	if (!old || (old->proc != new->proc)) {
		process_switch(new->proc);
	}

	/* load FPU state */
	if (new->fxdata) {
		fpu_load(new->fxdata);
	}

	return new;
}
