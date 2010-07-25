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

void		_done(void);
uint32_t    _send(uint32_t port, uint32_t target);
uintptr_t	_gvpr(uintptr_t address, uint32_t field);
uintptr_t   _svpr(uintptr_t address, uint32_t field);

#define VPR_FRAME	0
#define VPR_SOURCE	1
#define VPR_LENGTH	2
#define VPR_PORT	3

int32_t		_mmap(uintptr_t addr, uint32_t flags, uint32_t frame);

int32_t		_fork(void);
void		_exit(uint32_t value);
uint32_t	_pctl(uint32_t flags, uint32_t mask);
int         _exec(uintptr_t addr);
uint32_t    _gpid(uint32_t selector);

#define GPID_SELF	0
#define GPID_PARENT	1

#define MMAP_READ	0x001
#define MMAP_WRITE	0x002
#define MMAP_EXEC 	0x004
#define MMAP_FREE	0x008
#define MMAP_FRAME	0x010
#define MMAP_PHYS	0x020
#define MMAP_MOVE	0x040

#define CTRL_NONE		0x00000000
#define CTRL_BLOCK		0x00000001
#define CTRL_CLEAR		0x00000002
#define CTRL_ENTER		0x00000004
#define CTRL_SUPER		0x00000008
#define CTRL_PORTS		0x00000010
#define CTRL_IRQRD		0x00000020
#define CTRL_FLOAT		0x00000040
#define CTRL_RENICE		0x00000080
#define CTRL_CBLOCK		0x00001000
#define CTRL_CCLEAR		0x00002000
#define CTRL_IRQST		0x00004000
#define CTRL_DBLOCK		0x00010000
#define CTRL_DCLEAR		0x00020000
#define CTRL_ASYNC		0x00100000
#define CTRL_MULTI		0x00200000
#define CTRL_QUEUE		0x00400000
#define CTRL_MMCLR		0x00800000

#define CTRL_NICEMASK	0x00000F00
#define CTRL_NICE(n)	(((n) & 0xF) << 8)
#define CTRL_IRQMASK	0xFF000000
#define CTRL_IRQ(n)		(((n) & 0xFF) << 24)

#endif/*ABI_H*/
