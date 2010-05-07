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
/* If an IRQ was held, it is redirected now */

uint32_t tick = 0;

thread_t *pit_handler(thread_t *image) {

	tick++;

	/* Switch to next scheduled task */
	return thread_switch(image, schedule_next());
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

	#ifdef PARANOID
	extern uint32_t get_cr2(void);
	uint32_t cr2;

	/* If in kernelspace, panic */
//	if ((image->cs & 0x3) == 0) { /* i.e. if it was kernelmode */	
		/* Get faulting address from register CR2 */
		cr2 = get_cr2();

		printk("page fault at %x, ip = %x frame %x\n", 
			cr2, image->eip, page_get(cr2));

		panic("page fault exception");
//	}
	#endif

	return thread_fire(image, image->proc->pid, SSIG_PAGE, 0);
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

thread_t *syscall_fire(thread_t *image) {
	uint32_t   targ  = image->eax;
	uint32_t   sig   = image->ecx;
	uint32_t   grant = image->ebx;

	if (targ == 0) {
		image->eax = 0;
		return thread_switch(image, schedule_next());
	}

	if (!process_get(targ)) {
		image->eax = ERROR;
		return image;
	}
	else {
		image->eax = 0;
	}

	return thread_fire(image, targ, sig, grant);
}

thread_t *syscall_drop(thread_t *image) {
	return thread_drop(image);
}

thread_t *syscall_sctl(thread_t *image) {
	uint32_t action = image->eax;
	uint32_t signal = image->ecx % 32;
	uint32_t handle = image->edx;
	uint32_t policy = image->edx;

	switch (action) {
	default :
	case 0:
		image->eax = image->proc->signal_policy[signal];
		image->proc->signal_policy[signal] = policy;
		break;
	case 1:
		image->eax = image->proc->signal_handle;
		image->proc->signal_handle = handle;
		break;
	}

	return image;
}

thread_t *syscall_mail(thread_t *image) {
	uint32_t signal = image->ecx % 32;
	uint32_t source = image->edx;
	uint32_t insert = image->eax;
	struct signal_queue *sq, *found;
	struct signal_queue **mailbox_in;
	struct signal_queue **mailbox_out;

	mailbox_in  = &image->proc->mailbox_in [signal];
	mailbox_out = &image->proc->mailbox_out[signal];

	if (!insert) {

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

		image->signal = sq->signal;
		image->grant  = sq->grant;
		image->source = sq->source;

		image->eax    = sq->grant;
		heap_free(sq, sizeof(struct signal_queue));
	}
	else {

		sq = heap_alloc(sizeof(struct signal_queue));

		sq->next = NULL;
		sq->signal = image->signal;
		sq->grant  = image->grant;
		sq->source = image->source;

		if (*mailbox_out) {
			(*mailbox_in)->next = sq;
			*mailbox_in = sq;
		}
		else {
			*mailbox_out = sq;
			*mailbox_in = sq;
		}
	}

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

	mem_free(0, ESPACE);

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
		if ((image->proc->flags & CTRL_SUPER) || image->grant == frame) {
			frame_free(page_ufmt(page_get(addr)));
			page_set(addr, page_fmt(frame, pflags));
			if (image->grant == frame) {
				image->grant = 0;
			}
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
