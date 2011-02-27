/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <interrupt.h>
#include <process.h>
#include <debug.h>

/*****************************************************************************
 * syscall_vm86 (int 0x52)
 *
 * ECX: ip | cs << 16
 * EDX: sp | ss << 16
 * 
 * Switches a thread to virtual 8086 mode. The processor will only be able to 
 * access the first 1 MB of virtual memory in this mode. The processor's 
 * registers are set to the given values. The thread can switch back to
 * protected mode only by triggering software interrupt 48 (30h). At this
 * point, the processor will return from this syscall.
 *
 * Note that the first 1 MB of memory is always untouched by the kernel, so
 * identity mapping it for BIOS calls is fine.
 */

struct thread *syscall_vm86(struct thread *image) {
	uint16_t ip, cs, sp, ss;
	
	if (image->user != 0) {
		image->eax = 1;
		return image;
	}

	ip = image->ecx & 0xFFFF;
	cs = image->ecx >> 16;
	sp = image->edx & 0xFFFF;
	ss = image->edx >> 16;

	image->vm86_saved_eip    = image->eip;
	image->vm86_saved_esp    = image->useresp;
	image->vm86_saved_eflags = image->eflags;

	image->cs      = cs;
	image->ss      = ss;
	image->useresp = sp;
	image->eip     = ip;

	image->ds      = 0;
	image->vm86_es = 0;
	image->vm86_ds = 0;
	image->vm86_fs = 0;
	image->vm86_gs = 0;

	image->vm86_active = 1;
	set_int_stack(&image->vm86_start);

	return image;
}
