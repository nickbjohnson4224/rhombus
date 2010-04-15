/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <util.h>
#include <time.h>
#include <space.h>

/***** IRQ HANDLERS *****/

static uint8_t held_irq[256];
static uint16_t irq_holder[256];
static uint16_t held_count;

/* Handles IRQ 0, and advances a simple counter used as a clock */
/* If an IRQ was held, it is redirected now */

uint32_t tick = 0;

thread_t *pit_handler(thread_t *image) {
	size_t i;
	process_t *holder;

	if (image->cs | 1) tick++;

	if (held_count) {
		for (i = 0; i < 15; i++) {
			if (held_irq[i]) {
				holder = process_get(irq_holder[i]);
				if (!holder || 
					(holder->flags & CTRL_CLEAR) || 
					(holder->sigflags & CTRL_SIRQ)) {
					continue;
				}
				held_irq[i]--;
				held_count--;
				return thread_fire(image, irq_holder[i], SSIG_IRQ, 0);
			}
		}
	}

	/* Switch to next scheduled task */
	return process_switch(task_next(0), 0);
}

thread_t *irq_redirect(thread_t *image) {
	process_t *holder;

	holder = process_get(irq_holder[DEIRQ(image->num)]);
	if ((holder->flags & CTRL_CLEAR) || (holder->flags & CTRL_SIRQ)) {
		held_irq[DEIRQ(image->num)]++;
		held_count++;
		return image;
	}

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

	return thread_fire(image, curr_pid, SSIG_FAULT, 0);
}

/* Page fault */
thread_t *fault_page(thread_t *image) {

	#ifdef PARANOID
	extern uint32_t get_cr2(void);
	uint32_t cr2;

	/* Get faulting address from register CR2 */
	cr2 = get_cr2();

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) { /* i.e. if it was kernelmode */
		printk("page fault at %x, ip = %x frame %x task %d\n", 
			cr2, image->eip, page_get(cr2), curr_pid);
		panic("page fault exception");
	}
	#endif

	return thread_fire(image, curr_pid, SSIG_PAGE, 0);
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

	return thread_fire(image, curr_pid, SSIG_FLOAT, 0);
}

/* Double fault */
thread_t *fault_double(thread_t *image) {

	/* Can only come from kernel problems */
	printk("DS:%x CS:%x\n", image->ds, image->cs);
	panic("double fault exception");
	return NULL;

}

/* Coprocessor Existence Fault */
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
/* See section II of the Flux manual for details */

thread_t *fire(thread_t *image) {
	uint32_t   targ  = image->eax;
	uint32_t   sig   = image->ecx;
	uint32_t   grant = image->ebx;
	uint32_t   flags = image->edx;
	process_t *dst_t = process_get(targ);

	if (targ == 0) {
		return process_switch(task_next(0), 0);
	}

	if (!dst_t || !dst_t->shandler || (dst_t->sigflags & (1 << sig))) {
		image->eax = ERROR;
		return image;
	}
	else {
		image->eax = 0;
	}

	if (flags & 0x1) {
		image = thread_drop(image);
	}

	return thread_fire(image, targ, sig, grant);
}

thread_t *drop(thread_t *image) {
	return thread_drop(image);
}

thread_t *hand(thread_t *image) {
	uint32_t old_handler;

	old_handler = curr_task->shandler;
	curr_task->shandler = image->eax;

	image->eax = old_handler;
	return image;
}

thread_t *ctrl(thread_t *image) {
	extern uint32_t can_use_fpu;
	uint32_t flags = image->eax;
	uint32_t mask  = image->edx;
	uint32_t space = image->ecx;
	uint8_t irq;

	switch (space) {
	case CTRL_PSPACE:

		/* Stop the modification of protected flags if not super */
		if ((curr_task->flags & CTRL_SUPER) == 0) {
			mask &= CTRL_SMASK;
		}

		/* Set flags */
		curr_task->flags = (curr_task->flags & ~mask) | (flags & mask);

		/* Unset CTRL_FLOAT if FPU is disabled */
		if ((flags & mask & CTRL_FLOAT) && (can_use_fpu == 0)) {
			curr_task->flags &= ~CTRL_FLOAT;
		}

		/* Update IRQ redirect if CTRL_IRQRD is changed */
		if (mask & CTRL_IRQRD) {
			if (flags & CTRL_IRQRD) {
				/* Set IRQ redirect */
				irq = (flags >> 24) & 0xFF;
				if (irq < 15) {
					irq_holder[irq] = curr_pid;
					register_int(IRQ(irq), irq_redirect);
					pic_mask(1 << irq);
				}
			}
			else {
				/* Unset IRQ redirect */
				irq = (curr_task->flags >> 24) & 0xFF;
				irq_holder[irq] = 0;
				register_int(IRQ(irq), NULL);
				pic_mask(1 << irq);
			}
		}

		image->eax = curr_task->flags;
		return image;

	case CTRL_SSPACE:
		
		/* Set flags */
		curr_task->sigflags = (curr_task->sigflags & ~mask) | (flags & mask);

		image->eax = curr_task->sigflags;
		return image;

	default:
		image->eax = ERROR;
		return image;
	}
}

thread_t *info(thread_t *image) {
	uint8_t sel = image->eax;

	switch (sel) {
	case 0: image->eax = image->proc->pid; break;
	case 1: image->eax = image->proc->parent; break;
	case 2: image->eax = tick; break;
	case 3: image->eax = 0x0003; break;
	case 4: image->eax = KSPACE; break;
	case 5: 
		if (image->proc->flags & CTRL_SUPER) {
			image->eax = CTRL_CMASK;
		}
		else {
			image->eax = CTRL_SMASK & CTRL_CMASK;
		}
		break;
	case 6: image->eax = MMAP_CMASK; break;
	case 7: image->eax = image->proc->sigflags;
	default: image->eax = -1;
	}

	return image;
}

thread_t *mmap(thread_t *image) {
	uintptr_t addr;
	uintptr_t count;
	uintptr_t flags;
	uintptr_t frame;
	uintptr_t pflags;

	addr  = image->ebx;
	count = image->ecx;
	flags = image->edx & 0xFFF;
	frame = image->edx &~0xFFF;

	if (addr & 0xFFF || addr >= KSPACE || count > 1024) {
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

thread_t *fork(thread_t *image) {
	pid_t parent;
	task_t *child;

	/* Save current PID - it will become the parent */
	parent = curr_pid;

	/* Create a new task from the parent */
	child = process_clone(process_get(parent));

	if (!child) {
		image->eax = 0;
		return image;
	}

	/* (still in parent) Set return value to child's PID */
	image->eax = child->pid;

	/* Switch to child */
	image = process_switch(child, 0);

	/* (now in child) Set return value to negative parent's PID */
	image->eax = (uint32_t) -parent;

	return image;
}

thread_t *exit(thread_t *image) {
	pid_t catcher;
	uint32_t ret_val;
	process_t *t;

	/* Send death signal to parent */
	catcher = curr_task->parent;

	/* Copy return value because images are cleared with the address space */
	ret_val = image->eax;

	/* If init exits, halt */
	if (curr_pid == 1) {
		panic("init died");
	}

	process_switch(process_get(catcher), 0);

	/* Deallocate current address space and clear metadata */
	space_free(curr_task->space);	/* Deallocate whole address space */
	process_kill(curr_task);		/* Clear metadata and relinquish PID */

	t = process_get(catcher);
	if (!t || !t->shandler || t->flags & CTRL_CLEAR) {
		/* Parent will not accept - reschedule */
		return process_switch(task_next(0), 0);
	}
	else {
		/* Send S_DTH signal to parent with return value */
		return thread_fire(image, catcher, SSIG_DEATH, 0);
	}
}
