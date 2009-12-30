/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <int.h>
#include <task.h>
#include <mem.h>

/***** IRQ HANDLERS *****/

/* Handles IRQ 0, and advances a simple counter used as a clock */
uint32_t tick = 0;
image_t *pit_handler(image_t *image) {
	if (image->cs | 1) tick++;

	/* Switch to next scheduled task */
	return task_switch(task_next(0));
}

image_t *irq_redirect(image_t *image) {

	/* Send S_IRQ signal to the task registered with the IRQ */
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, NULL, TF_NOERR);
}

/***** FAULT HANDLERS *****/

/* Generic fault */
image_t *fault_generic(image_t *image) {

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}

	/* If in userspace, redirect to signal S_GEN */
	return signal(curr_pid, S_GEN, NULL, TF_NOERR | TF_EKILL);
}

/* Page fault */
image_t *fault_page(image_t *image) {
	uint32_t cr2;

	/* Get faulting address from register CR2 */
	asm volatile ("movl %%cr2, %0" : "=r" (cr2));

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) { /* i.e. if it was kernelmode */
		printk("page fault at %x, ip = %x frame %x task %d\n", 
			cr2, image->eip, page_get(cr2), curr_pid);
		panic("page fault exception");
	}

	/* If in userspace, redirect to signal S_PAG, with faulting address */
	return signal(curr_pid, S_PAG, NULL, TF_NOERR | TF_EKILL);
}

/* Floating point exception */
image_t *fault_float(image_t *image) {

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		panic("floating point exception");
	}

	/* If in userspace, redirect to signal S_FPE */
	return signal(curr_pid, S_FPE, NULL, TF_NOERR | TF_EKILL);
}

/* Double fault */
image_t *fault_double(image_t *image) {

	/* Can only come from kernel problems */
	printk("DS:%x CS:%x\n", image->ds, image->cs);
	panic("double fault exception");
	return NULL;

}

/***** ABI 2 System Calls *****/
image_t *fire(image_t *image) {
	return signal(image->eax, image->ecx, (void*) image->ebx, 0);
}

image_t *drop(image_t *image) {
	return sret(image);
}

image_t *hand(image_t *image) {
	uint32_t old_handler;

	old_handler = curr_task->shandler;
	curr_task->shandler = image->eax;
	ret(image, old_handler);
}

image_t *ctrl(image_t *image) {
	uint32_t flags = image->eax;
	uint32_t mask = image->edx;
	uint8_t irq;

	if ((curr_task->flags & TF_SUPER) == 0) {
		mask &= TF_SMASK;
	}

	curr_task->flags = (curr_task->flags & ~mask) | (flags & mask);

	if (mask & TF_IRQRD) {
		if (flags & TF_IRQRD) {
			irq = (flags >> 24) & 0xFF;
			if (irq < 15) {
				irq_holder[irq] = curr_pid;
				register_int(IRQ(irq), irq_redirect);
			}
		}
		else {
			irq = (curr_task->flags >> 24) & 0xFF;
			irq_holder[irq] = 0;
			register_int(IRQ(irq), NULL);
		}
	}

	ret(image, curr_task->flags);
}

image_t *info(image_t *image) {
	uint8_t sel = image->eax;

	switch (sel) {
	case 0: ret(image, curr_pid);
	case 1: ret(image, curr_task->parent);
	case 2: ret(image, tick);
	case 3: ret(image, 2);
	default: ret(image, -1);
	}
}

image_t *mmap(image_t *image) {
	uintptr_t addr = image->ebx;
	size_t count =   image->ecx;
	uint16_t flags = image->edx & 0xFFF;
	uint32_t frame = image->edx & ~0xFFF;
	uint16_t pflags = 0;

	if (addr & 0xFFF) ret(image, -1);
	if (addr >= KSPACE) ret(image, -1);
	if (count > 1024) ret(image, -1);

	if (flags & 0x08) {
		mem_free(addr, count * PAGESZ);
		ret(image, 0);
	}
	
	pflags |= PF_USER;
	if (flags & 0x01) pflags |= PF_PRES;
	if (flags & 0x02) pflags |= PF_RW;
	if (flags & 0x04) pflags |= PF_PRES;

	if (flags & 0x10) {
		if ((curr_task->flags & TF_SUPER) || curr_task->grant == frame) {
			page_set(addr, page_fmt(frame, pflags));
			if (curr_task->grant == frame) {
				curr_task->grant = 0;
			}
			ret(image, 0);
		}
		else {
			ret(image, -1);
		}
	}

	if (flags & 0x20) {
		mem_alloc(addr, PAGESZ, pflags);
		ret(image, page_ufmt(page_get(addr)));
	}

	mem_alloc(addr, count * PAGESZ, pflags);
	ret(image, 0);
}

image_t *fork(image_t *image) {
	pid_t parent;
	task_t *child;

	/* Save current PID - it will become the parent */
	parent = curr_pid;

	/* Create a new task from the parent */
	child = task_new(task_get(parent));
	if (!child) ret(image, 0); /* Fail if child was not created */

	/* (still in parent) Set return value to child's PID */
	image->eax = child->pid;

	/* Switch to child */
	image = task_switch(child);

	/* (now in child) Set return value to negative parent's PID */
	image->eax = (uint32_t) -parent;

	return image;
}

image_t *exit(image_t *image) {
	pid_t catcher;
	uint32_t ret_val;
	task_t *t;

	/* Send death signal to parent */
	catcher = curr_task->parent;

	/* Copy return value because images are cleared with the address space */
	ret_val = image->eax;

	/* If init exits, halt */
	if (curr_pid == 1) {
		panic("init died");
	}

	/* Deallocate current address space and clear metadata */
	map_clean(curr_task->map);	/* Deallocate pages and page tables */
	map_free(curr_task->map);	/* Deallocate page directory itself */
	task_rem(curr_task);		/* Clear metadata and relinquish PID */

	t = task_get(catcher);
	if (!t || !t->shandler || t->flags & TF_SBLOK) {
		/* Parent will not accept - reschedule */
		return task_switch(task_next(0));
	}
	else {
		/* Send S_DTH signal to parent with return value */
		return signal(catcher, S_DTH, NULL, TF_NOERR);
	}
}
