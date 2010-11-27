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

#ifndef ABI_H
#define ABI_H

#include <stdint.h>

/* system calls *******************************************************/

void		_done(void) __attribute__ ((noreturn));
uint32_t    _send(uint32_t port, uint32_t target);
void        _when(uintptr_t entry);
void        _rirq(uint8_t irq);
uintptr_t	_gvpr(uintptr_t address, uint32_t field);
uintptr_t   _svpr(uintptr_t address, uint32_t field);

#define VPR_FRAME	0
#define VPR_SOURCE	1
#define VPR_LENGTH	2
#define VPR_PORT	3

int32_t		_mmap(uintptr_t addr, uint32_t flags, uint32_t frame);

#define MMAP_READ	0x001
#define MMAP_WRITE	0x002
#define MMAP_EXEC 	0x004
#define MMAP_FREE	0x008
#define MMAP_FRAME	0x010
#define MMAP_PHYS	0x020
#define MMAP_MOVE	0x040

int32_t		_fork(void);
void		_exit(uint32_t value) __attribute__ ((noreturn));
int         _exec(void);
uint32_t    _gpid(uint32_t selector);
uint32_t    _user(uint32_t pid);
int         _auth(uint32_t pid, uint32_t user);

#define GPID_SELF	0
#define GPID_PARENT	1
#define GPID_THREAD	2
#define GPID_TLS	3

#endif/*ABI_H*/
