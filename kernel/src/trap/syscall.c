// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

image_t *fault_generic(image_t *image) {
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	return signal(curr_pid, S_GEN, image->num, image->err, 0, 0);
}

image_t *fault_page(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("page fault exception");
	u32int cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
	return signal(curr_pid, S_PAG, cr2, image->err, 0, 0);
}

image_t *fault_float(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("floating point exception");
	return signal(curr_pid, S_FPE, image->eip, 0, 0, 0);
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
	image_t *tmp = signal(t->parent, S_DTH, image->eax, 0, 0, 0);
	map_clean(&t->map);
	map_free(&t->map);
	rem_task(get_task(dead_task));
	return tmp;
}

image_t *sint_call(image_t *image) {
	return signal(image->eax, image->esi, image->ebx, image->ecx, image->edx, image->edi);
}

image_t *sret_call(image_t *image) {
	return sret(image);
}

image_t *eout_call(image_t *image) {
	printk("%s", image->eax);
	return image;
}

image_t *irq_redirect(image_t *image) {
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, DEIRQ(image->num), 0, 0, 0);
}

image_t *rirq_call(image_t *image) {
	task_t *t = get_task(curr_pid);
	if (t->user.ring > 1) ret(image, EPERMIT);
	irq_holder[image->eax % 15] = curr_pid;
	register_int(IRQ(image->eax), irq_redirect);
	ret(image, 0);
}

image_t *mmap_call(image_t *image) {
	u32int page = image->eax;
	task_t *t = get_task(curr_pid);

	// Bounds check page address
	if (page > 0xF6000000) ret(image, EPERMIT);

	// Check for already allocated page
	if (page_get(&t->map, page) & 0x1) ret(image, EREPEAT);

	// Allocate page with flags
	p_alloc(&t->map, page, image->ebx | PF_PRES | PF_USER);
	u8int *test = (void*) page;
	*test = 0x42242442;
//	page_set(&t->map, page, page_fmt(frame_new(), image->ebx | PF_PRES));

	map_load(&t->map);
	ret(image, 0);
}

image_t *umap_call(image_t *image) {
	u32int page = image->eax;
	task_t *t = get_task(curr_pid);

	// Bounds check page address
	if (page > 0xF6000000) ret(image, EPERMIT);

	// Check for already freed page
	if ((page_get(&t->map, page) & 0x1) == 0) ret(image, EREPEAT);

	// Free page
	printk("freeing %x ", page_get(&t->map, page));
	frame_free(page_ufmt(page_get(&t->map, page)));
	page_set(&t->map, page, 0x00000000);
	printk("%x\n", page_get(&t->map, page));

	map_load(&t->map);
	ret(image, 0);
}

image_t *rmap_call(image_t *image) {
	u32int src = image->eax;
	u32int dest = image->ebx;
	u32int flags = image->ecx;
	task_t *t = get_task(curr_pid);

	// Bounds check both addresses
	if (src > 0xF6000000 || dest > 0xF6000000) ret(image, EPERMIT);

	// Check source
	if ((page_get(&t->map, src) & 0x1) == 0) ret(image, EREPEAT);

	// Check destination
	if (page_get(&t->map, dest) & 0x1) ret(image, EREPEAT);

	// Move page
	page_set(&t->map, dest, page_get(&t->map, src));
	page_set(&t->map, src, 0x00000000);
	
	map_load(&t->map);
	ret(image, 0);
}

image_t *fmap_call(image_t *image) {
	u32int src = image->ebx;
	u32int dest = image->ecx;
	task_t *t = get_task(curr_pid);
	task_t *src_t = get_task(image->eax);

	// Bounds check destination
	if (dest > 0xF6000000) ret(image, EPERMIT);

	// Set physical address if chosen (eax == 0)
	if (image->eax == 0) {
		page_set(&t->map, dest, src | PF_PRES | PF_USER | PF_RW);
		ret(image, 0);
	}

	// Bounds check source
	if (src < 0xF6000000 && t->user.ring > 0) ret(image, EPERMIT);

	// Check source
	if ((page_get(&src_t->map, src) & 0x1) == 0) ret(image, EREPEAT);

	// Check destination
	if (page_get(&t->map, dest) & 0x1) ret(image, EREPEAT);

	// Move page
	page_set(&t->map, dest, (page_get(&src_t->map, src) | PF_LINK));
	page_set(&src_t->map, src, (page_get(&src_t->map, src) | PF_REAL));

	ret(image, 0);
}
