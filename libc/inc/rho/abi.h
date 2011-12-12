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

#ifndef __RLIBC_ABI_H
#define __RLIBC_ABI_H

#include <stdint.h>
#include <stddef.h>

/* future ABI structure? **********************************************/

uint32_t kcall(uint32_t call, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);

// inter-process communication
#define KCALL_IPC_SEND			0x0000
#define kCALL_IPC_ENTRY			0x0001

// process management
#define KCALL_PROC_FORK			0x0100
#define KCALL_PROC_EXIT			0x0101
#define KCALL_PROC_KILL			0x0102
#define KCALL_PROC_GET_PID		0x0110
#define KCALL_PROC_SET_PID		0x0111
#define KCALL_PROC_GET_PPID		0x0112
#define KCALL_PROC_SET_PPID		0x0113
#define KCALL_PROC_GET_UID		0x0114
#define KCALL_PROC_SET_UID		0x0115
#define KCALL_PROC_SET_NAME		0x0116
#define KCALL_PROC_GET_NAME		0x0117

// thread management
#define KCALL_THREAD_SPAWN		0x0200
#define KCALL_THREAD_EXIT		0x0201
#define KCALL_THREAD_BLOCK		0x0202
#define KCALL_THREAD_UNBLOCK	0x0203
#define KCALL_THREAD_GET_ID		0x0210
#define KCALL_THREAD_GET_STACK	0x0211

// memory management
#define KCALL_MEM_FREE			0x0300
#define KCALL_MEM_ALLOC_ANON	0x0301
#define KCALL_MEM_ALLOC_PHYS	0x0302
#define KCALL_MEM_ALLOC_SELF	0x0303
#define KCALL_MEM_SET_PROT		0x0304
#define KCALL_MEM_GET_FRAME		0x0310

// kernel time
#define KCALL_TIME_KERNEL		0x0400
#define KCALL_TIME_CPU			0x0401
#define KCALL_TIME_PROC			0x0402
#define KCALL_TIME_THREAD		0x0403
#define KCALL_TIME_FREQ			0x0404

// architecture-specific
#define KCALL_ARCH_SET_IRQ		0x0F00
#define KCALL_ARCH_VM86			0x0F01

/* system calls *******************************************************/

int			_send(uintptr_t base, size_t count, uint8_t port, uint32_t target);
void		_done(void) __attribute__ ((noreturn));
void		_when(uintptr_t entry);
void		_rirq(uint8_t irq);
uint32_t	_also(uintptr_t function);
int			_stat(uint32_t tid);
int			_page(uintptr_t address, size_t count, uint32_t perm, int source, uintptr_t offset); 
uintptr_t	_phys(uintptr_t address);
int32_t		_fork(void);
void		__exit(uint32_t value) __attribute__ ((noreturn));
void		_stop(uint32_t tid);
void		_wake(uint32_t tid);
uint32_t	_gpid(uint32_t selector);
uint64_t	_time(uint32_t selector);
int         _proc(uint32_t pid, uint32_t selector, uint32_t value);
int         _kill(uint32_t target, uint8_t signal);
int         _name(char *name, uint32_t pid, uint32_t operation);
int         _rtab(uint32_t operation, uint64_t a, uint64_t b);
uint32_t    _reap(uint32_t pid);

#define GPID_SELF	0
#define GPID_PARENT	1
#define GPID_THREAD	2
#define GPID_TLS	3
#define GPID_USER	4
#define GPID_GROUP	5

#define TIME_KERNEL	0
#define TIME_CPU	1
#define TIME_PROC	2
#define TIME_THREAD	3
#define TIME_FREQ	4

#define PROC_READ_PPID	0
#define PROC_WRITE_PPID	1
#define PROC_READ_UID	2
#define PROC_WRITE_UID	3

#define RTAB_OPEN  0
#define RTAB_CLOSE 1

#endif/*__RLIBC_ABI_H*/
