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

#ifndef ABI_H
#define ABI_H

#include <stdint.h>
#include <stddef.h>

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
void		_exit(uint32_t value) __attribute__ ((noreturn));
void		_stop(uint32_t tid);
void		_wake(uint32_t tid);
uint32_t	_gpid(uint32_t selector);
uint64_t	_time(uint32_t selector);
uint32_t	_user(uint32_t pid);
int			_auth(uint32_t pid, uint32_t user);

#define GPID_SELF	0
#define GPID_PARENT	1
#define GPID_THREAD	2
#define GPID_TLS	3

#define TIME_KERNEL	0
#define TIME_CPU	1
#define TIME_PROC	2
#define TIME_THREAD	3
#define TIME_FREQ	4

#endif/*ABI_H*/
