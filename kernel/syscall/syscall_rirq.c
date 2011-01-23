/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <thread.h>
#include <irq.h>

/****************************************************************************
 * syscall_rirq (int 0x43)
 *
 * ECX: irq
 *
 * Sets the IRQ with number <irq> to be redirected to the current process
 * as a message on PORT_IRQ. If another IRQ number is set to be redirected,
 * that other IRQ number is disabled. This system call is privileged, and
 * returns nonzero on permission denied error.
 */

struct thread *syscall_rirq(struct thread *image) {

	if (image->proc->user != 0) {
		image->eax = 1;
		return image;
	}

	if (image->proc->rirq != IRQ_NULL) {
		irq_set_redirect(0, image->proc->rirq);
	}

	image->proc->rirq = image->ecx;
	irq_set_redirect(image->proc->pid, image->proc->rirq);	

	image->eax = 0;
	return image;
}
