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

#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include <process.h>
#include <stdint.h>
#include <types.h>
#include <ipc.h>

/* limits ******************************************************************/

#define MAX_THREADS 256

/* thread structure ********************************************************/

struct thread {

	/* stored continuation */
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t num;
	uint32_t err;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;

	uint32_t vm86_es;
	uint32_t vm86_ds;
	uint32_t vm86_fs;
	uint32_t vm86_gs;
	uint32_t vm86_start;
	uint32_t vm86_active;
	uint32_t vm86_if;
	uint32_t vm86_saved_eip;
	uint32_t vm86_saved_esp;
	uint32_t vm86_saved_eflags;

	uint32_t *fxdata;
	
	/* message packet */
	struct msg *msg;

	/* effective user id */
	uid_t user;

	/* owning process */
	struct process *proc;
	uintptr_t stack;
	tid_t id;

	/* scheduler information */
	uint64_t tick;
	struct thread *next;
	uint32_t frozen;

} __attribute__ ((packed));

/* thread operations *******************************************************/

struct thread *thread_alloc (void);
void           thread_free  (struct thread *thread);
struct thread *thread_switch(struct thread *old, struct thread *new);
struct thread *thread_send  (struct thread *image, pid_t target, portid_t port, struct msg *msg);
struct thread *thread_freeze(struct thread *image);
struct thread *thread_thaw  (struct thread *image);
struct thread *thread_exit  (struct thread *image);
uintptr_t      thread_bind  (struct thread *thread, struct process *proc);

/* scheduler ****************************************************************/

void           schedule_insert(struct thread *thread);
void           schedule_remove(struct thread *thread);
struct thread *schedule_next  (void);

#endif/*KERNEL_THREAD_H*/
