// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

// Handles IRQ 0, and advances a simple counter used as a clock
image_t *pit_handler(image_t *state) {
	static uint32_t tick = 0;
	if (state->cs & 0x3) tick++;

	return task_switch(task_next(0));
}

image_t *fault_generic(image_t *image) {
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	return signal(curr_pid, S_GEN, image->num | (image->err << 16), 0, 0, 0, TF_NOERR | TF_EKILL);
}

image_t *fault_page(image_t *image) {
	uint32_t cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
	if ((image->cs & 0x3) == 0) { // i.e. if it was kernelmode
		printk("page fault at %x, frame %x, ip = %x\n", cr2, page_get(cr2), image->eip);
		panic("page fault exception");
	}
	return signal(curr_pid, S_PAG, 0, page_get(cr2), 0, cr2, TF_NOERR | TF_EKILL);
}

image_t *fault_float(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("floating point exception");
	return signal(curr_pid, S_FPE, image->eip, 0, 0, 0, TF_NOERR | TF_EKILL);
}

image_t *fault_double(image_t *image) {
	printk("DS:%x CS:%x\n", image->ds, image->cs);
	panic("double fault exception");
	return NULL;
}

// Note - different than UNIX / POSIX fork() -
// parent gets child PID, child gets *negative* parent PID, 0 is error
// I think it's much more useful, but the libc can convert it easily

image_t *fork_call(image_t *image) {
	pid_t parent = curr_pid;
	task_t *child = task_new(task_get(curr_pid));
	if (!child) ret(image, 0);
	image->eax = child->pid;
	image = task_switch(child);
	ret(image, -parent);
}

image_t *exit_call(image_t *image) {
	uint32_t dead_task = curr_pid;
	uint32_t ret_val = image->eax;
	if (dead_task == 1) {
		asm volatile ("sti");
		asm volatile ("hlt");
	}
	task_t *t = task_get(dead_task);
	map_clean(t->map);
	map_free(t->map);
	task_rem(task_get(dead_task));
	return signal(t->parent, S_DTH, ret_val, 0, 0, 0, TF_NOERR);
}

image_t *sint_call(image_t *image) {
	return signal(image->edi, image->esi & 0xFF, 
		image->eax, image->ebx, image->ecx, image->edx, (image->esi >> 8) & 0xFF);
}

image_t *sret_call(image_t *image) {
	return sret(image);
}

image_t *irq_redirect(image_t *image) {
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, DEIRQ(image->num), 0, 0, 0, TF_NOERR);
}

image_t *rirq_call(image_t *image) {
	irq_holder[image->eax % 15] = curr_pid;
	register_int(IRQ(image->eax), irq_redirect);
	ret(image, 0);
}

image_t *lirq_call(image_t *image) {
	irq_holder[image->eax % 15] = 0;
	register_int(IRQ(image->eax), 0);
	ret(image, 0);
}

image_t *rsig_call(image_t *image) {
	task_t *t = task_get(curr_pid);
	uint32_t old_handler = 0;

	switch (signal_map[image->edi]) {
		case SF_SYS:
			if (!(t->flags & TF_SUPER)) ret(image, EPERMIT);
		case SF_USE:
			old_handler = signal_table[image->edi];
		case SF_NIL:
			signal_table[image->edi] = image->eax;
			signal_map[image->edi] = SF_USE;
			break;
	}

	ret(image, old_handler);
}

image_t *lsig_call(image_t *image) {
	task_t *t = task_get(curr_pid);
	uint32_t old_handler = 0;

	switch (signal_map[image->edi]) {
		case SF_SYS:
			if (!(t->flags & TF_SUPER)) ret(image, EPERMIT);
		case SF_USE:
			old_handler = signal_table[image->edi];
		default:
			signal_table[image->edi] = 0;
			signal_map[image->edi] = SF_NIL;
			break;
	}

	ret(image, old_handler);
}

image_t *mmap_call(image_t *image) {
	uint32_t dst = image->edi;

	// Bounds check page address
	if (dst + image->ecx > SIG_TBL) ret(image, EPERMIT);

	// Allocate pages with flags
	for (dst &= ~0xFFF; dst < (image->edi + image->ecx); dst += 0x1000) {
		if (!(page_get(dst) & 0x1)) {
			uint32_t page = page_fmt(frame_new(), (image->ebx & PF_MASK) | PF_PRES | PF_USER); 
			page_set(dst, page);
		}
		else page_set(dst, page_fmt(page_get(dst), (image->ebx & PF_MASK) | PF_PRES | PF_USER));
	}

	ret(image, 0);
}

image_t *umap_call(image_t *image) {
	uint32_t dst = image->edi;

	// Bounds check page address
	if (dst + image->ecx > SIG_TBL) ret(image, EPERMIT);

	// Free pages
	for (dst &= ~0xFFF; dst < (image->edi + image->ecx); dst += 0x1000) {
		if (page_get(dst) & PF_PRES) {
			if (page_get(dst) & PF_LINK) page_set(dst, 0);
			else p_free(dst);
		}
	}

	ret(image, 0);
}

image_t *push_call(image_t *image) {
	uint32_t src = image->esi;
	uint32_t dst = image->edi;
	uint32_t size = image->ecx;
	uint32_t targ = image->eax;
	task_t *t;
	uint32_t i;

	// Bounds check addresses
	if (src + size > LSPACE || dst + size > LSPACE) ret(image, EPERMIT);
	if (size > 0x4000) ret(image, EPERMIT);	// Limit writes to 16K

	// Find and check target
	if (targ) {
		t = task_get(targ);
		if (!t) ret(image, ENOTASK);
		map_temp(t->map);
	}

	// Map pages
	for (i = 0; i < size; i += 0x1000) {
		if (targ) page_set((uint32_t) &tdst[i], page_fmt(ttbl[(dst + i) >> 12], (PF_RW | PF_PRES)));
		else page_set((uint32_t) &tdst[i], page_fmt((dst + i) &~ 0xFFF, (PF_RW | PF_PRES)));
		page_set((uint32_t) &tsrc[i], page_fmt(ctbl[(src + i) >> 12], (PF_RW | PF_PRES)));
	}

	// Copy memory
	memcpy(&tdst[dst & 0xFFF], &tsrc[src & 0xFFF], size);
	
	ret(image, 0);
}

image_t *pull_call(image_t *image) {
	uint32_t dst = image->esi;
	uint32_t src = image->edi;
	uint32_t size = image->ecx;
	uint32_t targ = image->eax;
	task_t *t;
	uint32_t i;

	// Bounds check addresses
	if (src + size > LSPACE || dst + size > LSPACE) ret(image, EPERMIT);
	if (size > 0x4000) ret(image, EPERMIT);	// Limit reads to 16K

	// Find and check target
	if (targ) {
		t = task_get(targ);
		if (!t) ret(image, ENOTASK);
		map_temp(t->map);
	}

	// Map pages
	for (i = 0; i < size; i += 0x1000) {
		if (targ) page_set((uint32_t) &tsrc[i], page_fmt(ttbl[(src + i) >> 12], (PF_RW | PF_PRES)));
		else page_set((uint32_t) &tsrc[i], page_fmt((src + i) &~ 0xFFF, (PF_RW | PF_PRES)));
		page_set((uint32_t) &tdst[i], page_fmt(ctbl[(dst + i) >> 12], (PF_RW | PF_PRES)));
	}

	// Copy memory
	memcpy(&tdst[dst & 0xFFF], &tsrc[src & 0xFFF], size);
	
	ret(image, 0);
}
