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

#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <arch.h>
#include <space.h>

/* limits ******************************************************************/

#define MAX_TASKS 1024
#define MAX_PID   1024

/* IPC *********************************************************************/

#define PORT_FAULT	0
#define PORT_IRQRD	1
#define PORT_FLOAT	2
#define PORT_DEATH	3

struct packet {
	uint32_t port;
	uint32_t frame;
	uint32_t source;
};

/* processes ***************************************************************/

struct process {

	/* address space */
	space_t space;

	/* various crap */
	uint32_t flags;
	uint32_t pid;

	struct process *next_task;
	struct process *parent;

	uintptr_t entry;
	
	/* threads */
	struct thread *thread[256];
};

void            process_init  (void);
struct process *process_get   (uint32_t pid);
struct process *process_alloc (void);
struct process *process_clone (struct process *parent, struct thread *active);
void            process_free  (struct process *proc);
void            process_kill  (struct process *proc);
void            process_freeze(struct process *proc);
void            process_thaw  (struct process *proc);
void            process_touch (uint32_t pid);
void            process_switch(struct process *proc);

/* process control space ***************************************************/

#define CTRL_PSPACE	0
#define CTRL_SSPACE 1

#define CTRL_CMASK	0xFF8000FF
#define CTRL_SMASK 	0x00773047
#define CTRL_RMASK	0x00000003

#define CTRL_READY 	0x00000000
#define CTRL_BLOCK 	0x00000001
#define CTRL_CLEAR 	0x00000002
#define CTRL_ENTER 	0x00000004
#define CTRL_SUPER 	0x00000008
#define CTRL_PORTS 	0x00000010
#define CTRL_IRQRD 	0x00000020
#define CTRL_FLOAT 	0x00000040
#define CTRL_RNICE 	0x00000080
#define CTRL_QUEUE	0x00400000
#define CTRL_MMCLR	0x00800000

#define CTRL_SFAULT	0x00000001
#define CTRL_SENTER	0x00000002
#define CTRL_SPAGE	0x00000004
#define CTRL_SIRQ	0x00000008
#define CTRL_SKILL	0x00000010
#define CTRL_SIMAGE	0x00000020
#define CTRL_SFLOAT	0x00000040
#define CTRL_SDEATH	0x00000080

/* scheduler ****************************************************************/

void           schedule_insert(struct thread *thread);
void           schedule_remove(struct thread *thread);
struct thread *schedule_next  (void);

/* threads *****************************************************************/

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
	uint32_t tss_start;
	uint32_t *fxdata;
	
	/* virtual packet register */
	struct packet *packet;

	/* owning process */
	struct process *proc;

	/* user stack segment base */
	uintptr_t stack;

	/* scheduler information */
	struct thread *next;
	uint32_t frozen;

} __attribute__ ((packed));

void           thread_init  (void);
struct thread *thread_alloc (void);
void           thread_free  (struct thread *thread);
struct thread *thread_switch(struct thread *old, struct thread *new);
struct thread *thread_send  (struct thread *image, uint16_t targ, uint16_t sig);
struct thread *thread_freeze(struct thread *image);
struct thread *thread_thaw  (struct thread *image);
struct thread *thread_exit  (struct thread *image);
uintptr_t      thread_bind  (struct thread *thread, struct process *proc);

/* interrupt handling ******************************************************/

typedef struct thread* (*handler_t) (struct thread *);
void register_int(uint8_t n, handler_t handler);
struct thread *int_handler(struct thread *image);
void tss_set_esp(uint32_t esp);

/* IRQs ********************************************************************/

struct thread *pit_handler(struct thread *image);

#define IRQ(n) (n + 32)
#define DEIRQ(n) (n - 32)
void pic_mask(uint16_t mask);

/* system calls ************************************************************/

struct thread *syscall_send(struct thread *image);
struct thread *syscall_done(struct thread *image);
struct thread *syscall_when(struct thread *image);
struct thread *syscall_recv(struct thread *image);
struct thread *syscall_gvpr(struct thread *image);
struct thread *syscall_svpr(struct thread *image);

struct thread *syscall_fork(struct thread *image);
struct thread *syscall_exit(struct thread *image);
struct thread *syscall_pctl(struct thread *image);
struct thread *syscall_exec(struct thread *image);
struct thread *syscall_gpid(struct thread *image);
struct thread *syscall_time(struct thread *image);

struct thread *syscall_mmap(struct thread *image);
struct thread *syscall_mctl(struct thread *image);

/* faults and exceptions ***************************************************/

struct thread *fault_generic(struct thread *image);
struct thread *fault_page   (struct thread *image);
struct thread *fault_float  (struct thread *image);
struct thread *fault_double (struct thread *image);
struct thread *fault_nomath (struct thread *image);

#endif
