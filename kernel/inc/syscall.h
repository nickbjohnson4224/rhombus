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

#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H

#include <interrupt.h>
#include <stdint.h>

#define SYSCALL_SEND	0x40
#define SYSCALL_DONE	0x41
#define SYSCALL_WHEN	0x42
#define SYSCALL_RIRQ	0x43
#define SYSCALL_GVPR	0x44
#define SYSCALL_SVPR	0x45
#define SYSCALL_PAGE	0x46
#define SYSCALL_PHYS	0x47
#define SYSCALL_FORK	0x48
#define SYSCALL_EXIT	0x49

#define SYSCALL_EXEC	0x4B
#define SYSCALL_GPID	0x4C
#define SYSCALL_TIME	0x4D
#define SYSCALL_USER	0x4E
#define SYSCALL_AUTH	0x4F

struct thread *syscall_send(struct thread *image);
struct thread *syscall_done(struct thread *image);
struct thread *syscall_when(struct thread *image);
struct thread *syscall_rirq(struct thread *image);
struct thread *syscall_gvpr(struct thread *image);
struct thread *syscall_svpr(struct thread *image);

struct thread *syscall_page(struct thread *image);
struct thread *syscall_phys(struct thread *image);

struct thread *syscall_fork(struct thread *image);
struct thread *syscall_exit(struct thread *image);

struct thread *syscall_exec(struct thread *image);
struct thread *syscall_gpid(struct thread *image);
struct thread *syscall_time(struct thread *image);
struct thread *syscall_user(struct thread *image);
struct thread *syscall_auth(struct thread *image);

#endif/*KERNEL_SYSCALL_H*/
