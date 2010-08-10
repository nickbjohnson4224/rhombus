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
uint64_t tick = 0;

struct thread *pit_handler(struct thread *image) {
	tick++;

	image = schedule_next();

	return image;
}

struct thread *irq_redirect(struct thread *image) {

	/* Send S_IRQ signal to the task registered with the IRQ */
	return thread_send(NULL, irq_holder[DEIRQ(image->num)], PORT_IRQRD);
}

/***** FAULT HANDLERS *****/

/* Generic fault */
struct thread *fault_generic(struct thread *image) {

	#ifdef PARANOID
	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	#endif

	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_FAULT);
}

/* Page fault */
struct thread *fault_page(struct thread *image) {
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
		process_freeze(image->proc);
		return thread_send(image, image->proc->pid, PORT_FAULT);
	}
}

/* Floating point exception */
struct thread *fault_float(struct thread *image) {

	#ifdef PARANOID
	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		printk("ip = %x\n", image->eip);
		panic("floating point exception");
	}
	#endif

	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_FLOAT);
}

/* Double fault */
struct thread *fault_double(struct thread *image) {

	/* Can only come from kernel problems */
	printk("DS:%x CS:%x\n", image->ds, image->cs);
	panic("double fault exception");
	return NULL;

}

/* Coprocessor Existence Failure */
struct thread *fault_nomath(struct thread *image) {
	extern void clr_ts(void);
	extern void fpu_load(void *fxdata);
	extern uint32_t can_use_fpu;

	if (!can_use_fpu) {
		process_freeze(image->proc);
		return thread_send(image, image->proc->pid, PORT_FLOAT);
	}

	if (image->fxdata) {
		fpu_load(image->fxdata);
	}

	clr_ts();

	return image;
}

/***** System Calls *****/

struct thread *syscall_send(struct thread *image) {
	uint32_t   target = image->ecx;
	uint32_t   port   = image->eax;

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

	return thread_send(image, target, port);
}

struct thread *syscall_done(struct thread *image) {
	return thread_exit(image);
}

struct thread *syscall_svpr(struct thread *image) {
	uintptr_t addr = image->eax;
	uint32_t field = image->ecx;

	if (field == 0) {
		if (image->packet) {
			if (image->packet->frame) {
				frame_free(image->packet->frame);
			}
		}
		else {
			image->packet = heap_alloc(sizeof(struct packet));
		}

		if (addr) {
			image->packet->frame  = page_get(addr);
			page_set(addr, page_fmt(frame_new(), image->packet->frame));
		}
		else {
			image->packet->frame  = 0;
		}

		image->eax = 0;
		return image;
	}
	
	image->eax = -1;
	return image;
}

struct thread *syscall_gvpr(struct thread *image) {
	uintptr_t addr = image->eax;
	uint32_t field = image->ecx;

	if (field == 0) {
		if (!image->packet || !image->packet->frame) {
			image->eax = 0;
		}

		else if (addr & 0xFFF || addr + PAGESZ >= KSPACE) {
			image->eax = -1;
		}

		else {
			if (page_get(addr) & PF_PRES) {
				frame_free(page_get(addr));
			}
		
			page_set(addr, page_fmt(image->packet->frame, PF_PRES|PF_USER|PF_RW));

			image->eax = 1;
		}
	}

	else if (field == 1) {
		if (!image->packet) {
			image->eax = 0;
		}
		else {
			image->eax = image->packet->source;
		}
	}

	else if (field == 2) {
		if (!image->packet || !image->packet->frame) {
			image->eax = 0;
		}
		else {
			image->eax = PAGESZ;
		}
	}

	else if (field == 3) {
		if (!image->packet) {
			image->eax = 0;
		}
		else {
			image->eax = image->packet->port;
		}
	}

	return image;
}

struct thread *syscall_fork(struct thread *image) {
	struct process *parent;
	struct process *child;

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

struct thread *syscall_exit(struct thread *image) {

	if (image->proc->pid == 1) {
		panic("init died");
	}

	process_switch(process_get(1));
	process_kill(image->proc);

	return thread_switch(image, schedule_next());
}

struct thread *syscall_pctl(struct thread *image) {
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

struct thread *syscall_gpid(struct thread *image) {
	
	switch (image->eax) {
	case 0: image->eax = image->proc->pid; break;
	case 1: image->eax = image->proc->parent->pid; break;
	case 2: image->eax = image->id; break;
	case 3: image->eax = image->stack; break;
	}

	return image;
}

struct thread *syscall_time(struct thread *image) {
	image->eax = (tick >> 0);
	image->edx = (tick >> 32);
	return image;
}

struct thread *syscall_exec(struct thread *image) {

	if (elf_check((void*) image->eax)) {
		image->eax = -1;
		return image;
	}

	mem_free(0, SSPACE);

	image->eip = elf_load((void*) image->eax);
	image->useresp = image->stack + SEGSZ;

	return image;
}

struct thread *syscall_mmap(struct thread *image) {
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

struct thread *syscall_mctl(struct thread *image) {
	return image;
}
