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

#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H

#include <interrupt.h>
#include <stdint.h>

#define SYSCALL_SEND	0x40
#define SYSCALL_DONE	0x41
#define SYSCALL_WHEN	0x42
#define SYSCALL_RIRQ	0x43
#define SYSCALL_ALSO	0x44
#define SYSCALL_STAT	0x45
#define SYSCALL_PAGE	0x46
#define SYSCALL_PHYS	0x47
#define SYSCALL_FORK	0x48
#define SYSCALL_EXIT	0x49
#define SYSCALL_STOP	0x4A
#define SYSCALL_WAKE	0x4B
#define SYSCALL_GPID	0x4C
#define SYSCALL_TIME	0x4D
#define SYSCALL_USER	0x4E
#define SYSCALL_AUTH	0x4F
#define SYSCALL_PGRP	0x50
#define SYSCALL_KILL	0x51
#define SYSCALL_VM86	0x52
#define SYSCALL_NAME	0x53

struct thread *syscall_send(struct thread *image);
struct thread *syscall_done(struct thread *image);
struct thread *syscall_when(struct thread *image);
struct thread *syscall_rirq(struct thread *image);
struct thread *syscall_also(struct thread *image);
struct thread *syscall_stat(struct thread *image);
struct thread *syscall_page(struct thread *image);
struct thread *syscall_phys(struct thread *image);
struct thread *syscall_fork(struct thread *image);
struct thread *syscall_exit(struct thread *image);
struct thread *syscall_stop(struct thread *image);
struct thread *syscall_wake(struct thread *image);
struct thread *syscall_gpid(struct thread *image);
struct thread *syscall_time(struct thread *image);
struct thread *syscall_user(struct thread *image);
struct thread *syscall_auth(struct thread *image);
struct thread *syscall_pgrp(struct thread *image);
struct thread *syscall_kill(struct thread *image);
struct thread *syscall_vm86(struct thread *image);
struct thread *syscall_name(struct thread *image);

#endif/*KERNEL_SYSCALL_H*/
