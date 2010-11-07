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

#include <debug.h>
#include <elf.h>
#include <ktime.h>
#include <space.h>
#include <cpu.h>

/* Generic fault */
struct thread *fault_generic(struct thread *image) {

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		debug_printf("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		debug_panic("unknown exception");
	}

	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_ILL);
}

/* Page fault */
struct thread *fault_page(struct thread *image) {
	uint32_t cr2;

	/* Get faulting address from register CR2 */
	cr2 = cpu_get_cr2();

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) { /* i.e. if it was kernelmode */	

		debug_printf("page fault at %x, ip = %x frame %x\n", 
			cr2, image->eip, page_get(cr2));

		debug_panic("page fault exception");
	}

	if (cr2 >= image->stack && cr2 < image->stack + SEGSZ) {
		/* allocate stack */
		mem_alloc(cr2 & ~0xFFF, PAGESZ, PF_PRES | PF_RW | PF_USER);
		return image;
	}
	else {
		/* fault */
		debug_printf("%d: page fault at %x, ip = %x\n", image->proc->pid, cr2, image->eip);
		debug_printf("user stack dump: (ebp = %x)\n", image->ebp);
		debug_dumpi((void*) image->useresp, 12);
		debug_panic("page fault exception");

		process_freeze(image->proc);
		return thread_send(image, image->proc->pid, PORT_PAGE);
	}
}

/* Floating point exception */
struct thread *fault_float(struct thread *image) {

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		debug_printf("ip = %x\n", image->eip);
		debug_panic("floating point exception");
	}

	debug_printf("float fault at ip = %x\n", image->eip);
	debug_panic("floating point exception");

	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_FLOAT);
}

/* Double fault */
struct thread *fault_double(struct thread *image) {

	/* Can only come from kernel problems */
	debug_printf("DS:%x CS:%x\n", image->ds, image->cs);
	debug_panic("double fault exception");
	return NULL;

}

/* Coprocessor Existence Failure */
struct thread *fault_nomath(struct thread *image) {
	extern uint32_t can_use_fpu;

	if (!can_use_fpu) {
		process_freeze(image->proc);
		return thread_send(image, image->proc->pid, PORT_ILL);
	}

	return image;
}
