// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

image_t *fault_generic(image_t *image) {
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	return signal(curr_pid, S_GEN, image->num | (image->err << 16), 0, 0, 0, 0);
}

image_t *fault_page(image_t *image) {
	u32int cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
	if ((image->cs & 0x3) == 0) { // i.e. if it was kernelmode
		printk("page fault at %x, frame %x, ip = %x\n", cr2, page_get(cr2), image->eip);
		panic("page fault exception");
	}
	return signal(curr_pid, S_PAG, 0, page_get(cr2), 0, cr2, 0);
}

image_t *fault_float(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("floating point exception");
	return signal(curr_pid, S_FPE, image->eip, 0, 0, 0, 0);
}

image_t *fault_double(image_t *image) {
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
	task_t *t = get_task(dead_task);
	image_t *tmp = signal(t->parent, S_DTH, image->eax, 0, 0, 0, 0);
	map_clean(t->map);
	map_free(t->map);
	rem_task(get_task(dead_task));
	return tmp;
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
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, DEIRQ(image->num), 0, 0, 0, 0);
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
	if (dst + image->ecx > 0xF8000000) ret(image, EPERMIT);

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
	if (dst + image->ecx > 0xF8000000) ret(image, EPERMIT);

	// Free pages
	for (dst &= ~0xFFF; dst < (image->edi + image->ecx); dst += 0x1000)
		if (page_get(dst) & 0x1) p_free(dst);

	ret(image, 0);
}

image_t *rmap_call(image_t *image) {
	u32int src = image->esi;
	u32int dst = image->edi;
	u32int flags = image->ebx;

	// Bounds check both addresses
	if (src + image->ecx > 0xF8000000 || dst + image->ecx > 0xF8000000) ret(image, EPERMIT);
	if (src & 0xFFF || dst & 0xFFF) ret(image, EPERMIT);

	// Move pages
	for (; dst<image->edi+image->ecx && src<image->esi+image->ecx; dst += 0x1000, src += 0x1000) {
		if ((page_get(src) & 0x1) && ((page_get(dst) & 0x1) == 0)) {
			page_set(dst, page_fmt(page_get(src), flags));
			page_set(src, 0);
		}
	}
	
	ret(image, 0);
}

image_t *fmap_call(image_t *image) {
	u32int src = image->esi;
	u32int dst = image->edi;
	u32int flags = image->ebx;
	task_t *t = get_task(curr_pid);
	task_t *src_t;

	// Bounds check destination
	if (dst > 0xF8000000) ret(image, EPERMIT);

	// Set physical address if chosen (eax == 0)
	if (image->eax == 0) {
		page_set(dst, src | (flags & PF_MASK));
		ret(image, 0);
	}

	// Bounds check source
	if (src < 0xF8000000 && t->user.ring > 0) ret(image, EPERMIT);

	// Map source map
	src_t = get_task(image->eax);
	map_temp(src_t->map);

	// Check source
	if ((ttbl[src >> 22] & 0x1) == 0) ret(image, EREPEAT);

	// Check destination
	if (ctbl[dst >> 22] & 0x1) ret(image, EREPEAT);

	// Move page
	page_set(dst, ttbl[src >> 22] | PF_LINK);
	ttbl[src >> 22] |= PF_REAL;

	ret(image, 0);
}
