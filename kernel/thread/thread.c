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

#include <ktime.h>
#include <space.h>
#include <debug.h>
#include <irq.h>
#include <timer.h>
#include <cpu.h>

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

	if (!new) {
		cpu_idle();
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

	/* switch interrupt stacks */
	if (new && (old != new)) {
		set_int_stack(&new->kernel_stack);
	}

	/* load FPU state */
	if (new->fxdata) {
		fpu_load(new->fxdata);
	}

	return new;
}
