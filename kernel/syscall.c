/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <elf.h>
#include <time.h>
#include <space.h>

/***** IRQ HANDLERS *****/

uint16_t irq_holder[256];

/* Handles IRQ 0, and advances a simple counter used as a clock */
uint32_t tick = 0;

thread_t *pit_handler(thread_t *image) {
	tick++;

	return schedule_next();
}

thread_t *irq_redirect(thread_t *image) {

	/* Send S_IRQ signal to the task registered with the IRQ */
	return thread_fire(image, irq_holder[DEIRQ(image->num)], SSIG_IRQ, 0);
}

/***** FAULT HANDLERS *****/

/* Generic fault */
thread_t *fault_generic(thread_t *image) {

	#ifdef PARANOID
	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	#endif

	return thread_fire(image, image->proc->pid, SSIG_FAULT, 0);
}

/* Page fault */
thread_t *fault_page(thread_t *image) {
	extern uint32_t get_cr2(void);
	uint32_t cr2;

	/* Get faulting address from register CR2 */
	cr2 = get_cr2();

	#ifdef PARANOID
	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) { /* i.e. if it was kernelmode */	

		printk("page fault at %x, ip = %x frame %x\n", 
			cr2, image->eip, page_get(cr2));

		panic("page fault exception");
	}
	#endif

	if (cr2 >= image->stack && cr2 < image->stack + SEGSZ) {
		/* allocate stack */
		mem_alloc(cr2 & ~0xFFF, PAGESZ, PF_PRES | PF_RW | PF_USER);
		return image;
	}
	else {
		/* fault */
		printk("page fault at %x, ip = %x frame %x proc %d\n", 
			cr2, image->eip, page_get(cr2), image->proc->pid);

		panic("page fault exception");
		return thread_fire(image, image->proc->pid, SSIG_FAULT, 0);
	}
}

/* Floating point exception */
thread_t *fault_float(thread_t *image) {

	#ifdef PARANOID
	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		printk("ip = %x\n", image->eip);
		panic("floating point exception");
	}
	#endif

	return thread_fire(image, image->proc->pid, SSIG_FLOAT, 0);
}

/* Double fault */
thread_t *fault_double(thread_t *image) {

	/* Can only come from kernel problems */
	printk("DS:%x CS:%x\n", image->ds, image->cs);
	panic("double fault exception");
	return NULL;

}

/* Coprocessor Existence Failure */
thread_t *fault_nomath(thread_t *image) {
	extern void clr_ts(void);
	extern void fpu_load(void *fxdata);

	if (image->fxdata) {
		fpu_load(image->fxdata);
	}

	clr_ts();

	return image;
}

/***** System Calls *****/

thread_t *syscall_send(thread_t *image) {
	uint32_t   target = image->eax;
	uint32_t   port   = image->ecx;
	uint32_t   packet = image->ebx;

	if (target == 0) {
		image->eax = 0;
		return schedule_next();
	}

	if (!process_get(target)) {
		image->eax = ERROR;
		return image;
	}
	else {
		image->eax = 0;
	}

	return thread_fire(image, target, port, packet);
}

thread_t *syscall_drop(thread_t *image) {
	return thread_drop(image);
}

thread_t *syscall_evnt(thread_t *image) {
	uint32_t port   = image->ecx & 0xFF;
	uint32_t handle = image->edx;

	image->eax = image->proc->port[port].entry;
	image->proc->port[port].entry = handle;

	return image;
}

thread_t *syscall_recv(thread_t *image) {
	uint32_t port   = image->ecx % 256;
	uint32_t source = image->edx;
	struct packet *sq, *found;
	struct packet **mailbox_in;
	struct packet **mailbox_out;

	mailbox_in  = &image->proc->port[port].in;
	mailbox_out = &image->proc->port[port].out;

	sq = *mailbox_out;

	if (!sq) {
		image->eax = -1;
		return image;
	}

	if (source) {
		if (sq->source == source) {
			*mailbox_out = (*mailbox_out)->next;
		}
		else {
			for (found = NULL; sq->next; sq = sq->next) {
				if (sq->next->source == source) {
					found = sq->next;
					sq->next = sq->next->next;
					
					if (*mailbox_in == found) {
						*mailbox_in = sq;
					}
	
					sq = found;
					break;
				}
			}

			if (!found) {
				image->eax = -1;
				return image;
			}
		}
	}
	else {
		*mailbox_out = (*mailbox_out)->next;
	}

	if (!(*mailbox_out)) {
		*mailbox_in = NULL;
	}

	image->packet = sq;

	if (sq->grant) {
		image->eax = 1;
	}
	else {
		image->eax = 0;
	}

	return image;
}

thread_t *syscall_pget(thread_t *image) {
	uintptr_t addr = image->eax;
	uintptr_t i;

	if (!image->packet || !image->packet->grant) {
		image->eax = 0;
		return image;
	}

	if (addr & 0xFFF || addr + PAGESZ >= KSPACE) {
		image->eax = -1;
		return image;
	}

	if (page_get(addr) & PF_PRES) {
		frame_free(page_get(addr));
	}

	page_set(addr, page_fmt(image->packet->grant, PF_PRES | PF_USER | PF_RW));

	image->eax = 1;
	return image;
}

thread_t *syscall_fork(thread_t *image) {
	process_t *parent;
	process_t *child;

	parent = image->proc;
	child = process_clone(parent, image);

	if (!child) {
		image->eax = 0;
		return image;
	}

	/* (still in parent) Set return value to child's PID */
	image->eax = child->pid;

	/* Switch to child */
	image = thread_switch(image, child->thread[0]);

	/* (now in child) Set return value to negative parent's PID */
	image->eax = -((uint32_t) parent->pid);

	return image;
}

thread_t *syscall_exit(thread_t *image) {
	process_t *parent;
	uint32_t ret_val;

	parent = image->proc->parent;

	/* Copy return value because images are cleared with the address space */
	ret_val = image->eax;

	/* If init exits, halt */
	if (image->proc->pid == 1) {
		panic("init died");
	}

	process_switch(process_get(1));

	/* Deallocate current address space and clear metadata */
	space_free(image->proc->space);	/* Deallocate whole address space */
	process_kill(image->proc);		/* Clear metadata and relinquish PID */

	if (!parent) {
		/* Parent will not accept - reschedule */
		return thread_switch(image, schedule_next());
	}
	else {
		/* Send SSIG_DEATH signal to parent with return value */
		return thread_fire(image, parent->pid, SSIG_DEATH, 0);
	}
}

thread_t *syscall_pctl(thread_t *image) {
	uint32_t flags = image->eax;
	uint32_t mask  = image->edx;
	uint8_t irq;

	/* Stop the modification of protected flags if not super */
	if ((image->proc->flags & CTRL_SUPER) == 0) {
		mask &= CTRL_SMASK;
	}

	/* Set flags */
	image->proc->flags = (image->proc->flags & ~mask) | (flags & mask);

	/* Update IRQ redirect if CTRL_IRQRD is changed */
	if (mask & CTRL_IRQRD) {
		if (flags & CTRL_IRQRD) {
			/* Set IRQ redirect */
			irq = (flags >> 24) & 0xFF;
			if (irq < 15) {
				irq_holder[irq] = image->proc->pid;
				register_int(IRQ(irq), irq_redirect);
				pic_mask(1 << irq);
			}
		}
		else {
			/* Unset IRQ redirect */
			irq = (image->proc->flags >> 24) & 0xFF;
			irq_holder[irq] = 0;
			register_int(IRQ(irq), NULL);
			pic_mask(1 << irq);
		}
	}

	image->eax = image->proc->flags;
	return image;
}

thread_t *syscall_exec(thread_t *image) {

	if (elf_check((void*) image->eax)) {
		image->eax = -1;
		return image;
	}

	mem_free(0, SSPACE);

	image->eip = elf_load((void*) image->eax);
	image->useresp = image->stack + SEGSZ;

	return image;
}

thread_t *syscall_mmap(thread_t *image) {
	uintptr_t addr;
	uintptr_t count;
	uintptr_t flags;
	uintptr_t frame;
	uintptr_t pflags;

	addr  = image->ebx;
	count = image->ecx;
	flags = image->edx & 0xFFF;
	frame = image->edx &~0xFFF;

	if (addr & 0xFFF || addr + (count * PAGESZ) >= KSPACE) {
		image->eax = ERROR;
		return image;
	}

	if (flags & MMAP_PHYS) {
		image->eax = page_ufmt(page_get(addr));
		return image;
	}

	if (flags & MMAP_FREE) {
		mem_free(addr, count * PAGESZ);
		image->eax = 0;
		return image;
	}
	
	pflags = PF_USER | PF_PRES | ((flags & MMAP_WRITE) ? PF_RW : 0);

	if (flags & MMAP_FRAME) {
		
		if ((image->proc->flags & CTRL_SUPER)) {
			if (page_get(addr) & PF_PRES) {
				frame_free(page_ufmt(page_get(addr)));
			}

			page_set(addr, page_fmt(frame, pflags));
			image->eax = 0;
		}
		else {
			image->eax = ERROR;
		}

		return image;
	}

	mem_alloc(addr, count * PAGESZ, pflags);

	image->eax = 0;
	return image;
}

thread_t *syscall_mctl(thread_t *image) {
	return image;
}
