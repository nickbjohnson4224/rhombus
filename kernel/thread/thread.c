/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#include <process.h>
#include <thread.h>
#include <space.h>
#include <debug.h>
#include <timer.h>
#include <irq.h>
#include <cpu.h>

/****************************************************************************
 * thread_alloc
 *
 * Returns a pointer to a thread that was not previously allocated. Returns
 * null on error. The thread structure is page aligned.
 */

struct thread *thread_alloc(void) {
	struct thread *thread;

	thread = heap_alloc(sizeof(struct thread));
	thread->fxdata = heap_alloc(512);

	return thread;
}

/****************************************************************************
 * thread_exit
 *
 * Kills the given thread and switches to another runnable thread.
 */

struct thread *thread_exit(struct thread *image) {
	thread_free(image);

	return thread_switch(NULL, schedule_next());
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

struct thread *thread_send(struct thread *image, pid_t target, portid_t port, struct msg *msg) {
	struct process *p_targ;
	struct thread *new_image;

	/* find target process */
	p_targ = process_get(target);

	/* check process */
	if (!p_targ || !p_targ->entry) {
		return image;
	}

	/* create new thread */
	new_image = thread_alloc();
	thread_bind(new_image, p_targ);

	new_image->ds      = 0x23;
	new_image->cs      = 0x1B;
	new_image->ss      = 0x23;
	new_image->eflags  = 0;
	new_image->useresp = new_image->stack + SEGSZ;
	new_image->proc    = p_targ;
	new_image->eip     = p_targ->entry;

	/* set up registers in new thread */
	new_image->ecx     = (msg) ? msg->count : 0;
	new_image->edx     = port;
	new_image->esi     = (image) ? image->proc->pid : 0;
	new_image->msg     = msg;

	/* set new thread's user id */
	new_image->user = (!image || p_targ->user) ? p_targ->user : image->user;

	/* insert new thread into scheduler */
	schedule_insert(new_image);

	/* return new thread */
	return new_image;
}

/****************************************************************************
 * thread_free
 *
 * Frees a thread and its associated memory.
 */

void thread_free(struct thread *thread) {
	uintptr_t i;

	/* free FPU/SSE data */
	if (thread->fxdata) {
		heap_free(thread->fxdata, 512);
		thread->fxdata = NULL;
	}

	/* remove thread from scheduler */
	schedule_remove(thread);

	/* free message packet if it exists */
	if (thread->msg) {
		
		/* free packet contents */
		for (i = 0; i < thread->msg->count; i++) {
			frame_free(thread->msg->frame[i]);
		}

		/* free the message packet structure */
		heap_free(thread->msg->frame, thread->msg->count * sizeof(uint32_t));
		heap_free(thread->msg, sizeof(struct msg));

	}

	/* free thread local storage if it exists */
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

	/* free thread structure */
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
	if (old && old->fxdata) {
		fpu_save(old->fxdata);
	}
	
	/* switch processes */
	if (!old || (old->proc != new->proc)) {
		process_switch(new->proc);
	}

	/* switch interrupt stacks */
	if (new && (old != new)) {
		if (new->vm86_active) {
			set_int_stack(&new->vm86_start);
		}
		else {
			set_int_stack(&new->vm86_es);
		}
	}

	/* load FPU state */
	if (new->fxdata) {
		fpu_load(new->fxdata);
	}

	return new;
}
