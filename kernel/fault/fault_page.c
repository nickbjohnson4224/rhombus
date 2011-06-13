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

#include <thread.h>
#include <string.h>
#include <debug.h>
#include <space.h>
#include <cpu.h>

/*****************************************************************************
 * fault_page
 *
 * Page fault handler. If the fault is from userspace, and from a TLS block,
 * memory is allocated to fix the issue; if not from a TLS block, the
 * process is frozen and sent a message on port PORT_PAGE. If the fault is
 * from kernel space, it panics.
 *
 * Note: lines that cause a panic with a stack dump on userspace faults are
 * commented out, but can be very useful for tracking userspace bugs, until I
 * write a proper debugger.
 */

struct thread *fault_page(struct thread *image) {
	uint32_t cr2;

	/* Get faulting address from register CR2 */
	cr2 = cpu_get_cr2();

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) { /* i.e. if it was kernelmode */	

		debug_printf("page fault at %x, ip = %x, frame %x, esp = %x\n", 
			cr2, image->eip, page_get(cr2), image->esp);

		debug_panic("page fault exception");
	}

	if (cr2 >= image->stack && cr2 < image->stack + SEGSZ) {
		/* allocate stack */
		mem_alloc(cr2 & ~0xFFF, PAGESZ, PF_PRES | PF_RW | PF_USER);
		return image;
	}
	else {
		/* fault */
		debug_printf("%d: page fault at %x, ip = %x, frame %x\n", 
			image->proc->pid, cr2, image->eip, page_get(cr2));
		debug_printf("user stack: %x - %x\n", image->stack, image->stack + SEGSZ);
		debug_printf("user stack dump: (ebp = %x)\n", image->ebp);
		debug_dumpi((void*) image->useresp, 30);
//		debug_panic("page fault exception");

		process_freeze(image->proc);
		return thread_send(image, image->proc->pid, PORT_PAGE, NULL);
	}
}
