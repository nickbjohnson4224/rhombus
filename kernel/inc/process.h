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

#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <stdint.h>
#include <thread.h>
#include <space.h>
#include <types.h>
#include <arch.h>

/* limits ******************************************************************/

#define MAX_PID     1024
#define MAX_TASKS	1024

/* key structure ***********************************************************/

struct key {
	uint64_t resource;
	uint32_t keyvalue;
};

/* process structure *******************************************************/

struct process {

	/* address space */
	space_t space;

	/* keys */
	uint32_t    keycount;
	struct key *keytable;

	/* various metadata */
	uint64_t tick;
	pid_t    pid;
	uid_t    user;
	intid_t  rirq;
	char     name[16];

	struct process *parent;

	/* message handler */
	uintptr_t entry;
	
	/* threads */
	struct thread *thread[256];
};

/* process operations ******************************************************/

struct process *process_get   (pid_t pid);
struct process *process_alloc (void);
struct process *process_clone (struct process *parent, struct thread *active);
void            process_free  (struct process *proc);
void            process_kill  (struct process *proc);
void            process_touch (pid_t pid);
void            process_switch(struct process *proc);

void            process_freeze(struct process *proc);
void            process_thaw  (struct process *proc);

/* process key operations ***************************************************/

void     key_set(struct process *proc, uint64_t resource, uint32_t keyvalue);
uint32_t key_get(struct process *proc, uint64_t resource);

#endif/*KERNEL_PROCESS_H*/
