// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

// Handles IRQ 0, and advances a simple counter used as a clock
image_t *pit_handler(image_t *state) {
	static u32int tick = 0;
	if (state->cs & 0x3) tick++;

	return task_switch(next_task(0));
}

image_t *fault_generic(image_t *image) {
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	return signal(curr_pid, S_GEN, image->num | (image->err << 16), 0, 0, 0, TF_NOERR | TF_EKILL);
}

image_t *fault_page(image_t *image) {
	u32int cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
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
	u16int parent = curr_pid;
	task_t *child = new_task(get_task(curr_pid));
	if (child->magic != 0x4224) ret(image, 0);
	image->eax = child->pid;
	image = task_switch(child);
	ret(image, -parent);
}

image_t *exit_call(image_t *image) {
	u16int dead_task = curr_pid;
	u32int ret_val = image->eax;
	if (dead_task == 1) {
		asm volatile ("sti");
		asm volatile ("hlt");
	}
	task_t *t = get_task(dead_task);
	map_clean(t->map);
	map_free(t->map);
	rem_task(get_task(dead_task));
	return signal(t->parent, S_DTH, ret_val, 0, 0, 0, TF_NOERR);
}

image_t *sint_call(image_t *image) {
	return signal(image->edi, image->esi & 0xFF, 
		image->eax, image->ebx, image->ecx, image->edx, (image->esi >> 8) & 0xFF);
}

image_t *sret_call(image_t *image) {
	return sret(image);
}

image_t *eout_call(image_t *image) {
	printk("%s", image->eax);
	return image;
}

image_t *irq_redirect(image_t *image) {
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, DEIRQ(image->num), 0, 0, 0, TF_NOERR);
}

image_t *rirq_call(image_t *image) {
	task_t *t = get_task(curr_pid);
	if (t->user.ring > 1) ret(image, EPERMIT);
	irq_holder[image->eax % 15] = curr_pid;
	register_int(IRQ(image->eax), irq_redirect);
	ret(image, 0);
}

image_t *mmap_call(image_t *image) {
	u32int dst = image->edi;

	// Bounds check page address
	if (dst + image->ecx > LSPACE) ret(image, EPERMIT);

	// Allocate pages with flags
	for (dst &= ~0xFFF; dst < (image->edi + image->ecx); dst += 0x1000) {
		if (!(page_get(dst) & 0x1)) {
			u32int page = page_fmt(frame_new(), (image->ebx & PF_MASK) | PF_PRES | PF_USER); 
			page_set(dst, page);
		}
	}

	ret(image, 0);
}

image_t *umap_call(image_t *image) {
	u32int dst = image->edi;

	// Bounds check page address
	if (dst + image->ecx > LSPACE) ret(image, EPERMIT);

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
	u32int src = image->esi;
	u32int dst = image->edi;
	u32int size = image->ecx;
	u32int targ = image->eax;
	task_t *t, *src_t;
	u32int soff, doff, cpy_size;

	// Bounds check addresses
	if (src + size > LSPACE || dst + size > LSPACE) ret(image, EPERMIT);

	// Get offsets
	soff = src & 0xFFF;
	doff = dst & 0xFFF;

	// Find and check target
	if (targ) {
		t = get_task(targ);
		if (!t) ret(image, ENOTASK);
		map_temp(t->map);
	}
	else {
		src_t = get_task(curr_pid);
		if (src_t->user.ring != 0) ret(image, EPERMIT);
	}

	// Copy memory
	while (size) {
		page_set((u32int) tsrc, page_fmt(ctbl[src >> 12], (PF_RW | PF_PRES)));
		if (targ)
			page_set((u32int) tdst, page_fmt(ttbl[dst >> 12], (PF_RW | PF_PRES)));
		else
			page_set((u32int) tdst, page_fmt((dst &~ 0xFFF), (PF_RW | PF_PRES)));
		cpy_size = min(size, min((0x1000 - soff), (0x1000 - doff)));
		memcpy(&tdst[doff], &tsrc[soff], cpy_size);
		doff += cpy_size;
		soff += cpy_size;
		size -= cpy_size;
		if (doff >= 0x1000) {
			dst += 0x1000;
			doff = 0;
		}
		if (soff >= 0x1000) {
			src += 0x1000;
			soff = 0;
		}
	}

	ret(image, 0);
}

image_t *pull_call(image_t *image) {
	u32int dst = image->esi;
	u32int src = image->edi;
	u32int size = image->ecx;
	u32int targ = image->eax;
	task_t *t, *src_t;
	u32int soff, doff, cpy_size;

	// Bounds check addresses
	if (src + size > LSPACE || dst + size > LSPACE) ret(image, EPERMIT);

	// Get offsets
	soff = src & 0xFFF;
	doff = dst & 0xFFF;

	// Find and check target
	if (targ) {
		t = get_task(targ);
		if (!t) ret(image, ENOTASK);
		map_temp(t->map);
	}
	else {
		src_t = get_task(curr_pid);
		if (src_t->user.ring != 0) ret(image, EPERMIT);
	}

	// Copy memory
	while (size) {
		if (targ)
			page_set((u32int) tsrc, page_fmt(ttbl[src >> 12], (PF_RW | PF_PRES)));
		else
			page_set((u32int) tsrc, page_fmt((src &~ 0xFFF), (PF_RW | PF_PRES)));
		page_set((u32int) tdst, page_fmt(ctbl[dst >> 12], (PF_RW | PF_PRES)));
		cpy_size = min(size, min((0x1000 - soff), (0x1000 - doff)));
		memcpy(&tdst[doff], &tsrc[soff], cpy_size);
		doff += cpy_size;
		soff += cpy_size;
		size -= cpy_size;
		if (doff >= 0x1000) {
			dst += 0x1000;
			doff = 0;
		}
		if (soff >= 0x1000) {
			src += 0x1000;
			soff = 0;
		}
	}
	
	ret(image, 0);
}
