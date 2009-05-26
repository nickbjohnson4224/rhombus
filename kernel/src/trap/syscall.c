// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

image_t *fault_generic(image_t *image) {
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	return ksignal(0, curr_pid, S_GEN, image->num, image->err, 0);
}

image_t *fault_page(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("page fault exception");
	u32int cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
	return ksignal(0, curr_pid, S_PAG, cr2, image->err, 0);
}

image_t *fault_float(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("floating point exception");
	return ksignal(0, curr_pid, S_FPE, image->eip, 0, 0);
}

image_t *fault_double(image_t *image) {
	panic("double fault exception");
	return NULL;
}

image_t *fork_call(image_t *image) {
	image->eax = 0;
	u16int parent = curr_pid;
	image = task_switch(new_task(get_task(curr_pid)));
	image->eax = parent;
	return image;
}

image_t *exit_call(image_t *image) {
	printk("exit() %d\n", curr_pid);

	u16int dead_task = curr_pid;
	task_t *t = get_task(dead_task);
	image_t *tmp = ksignal(dead_task, t->parent, S_DTH, image->eax, 0, 0);
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

image_t *mmap_call(image_t *image) {
	printk("mmap(%x, %x)\n", image->eax, image->ebx);

	u32int page = image->eax;
	u32int flags = image->ebx;
	task_t *t = get_task(curr_pid);

	// Bounds check page address
	if (page > 0xF6000000) {
		image->eax = EPERMIT;
		return image;
	}

	// Check for already allocated page
	if (page_get(&t->map, page) & 0x1) {
		image->eax = EREPEAT;
		return image;
	}

	// Allocate page with flags
	page_set(&t->map, page, page_fmt(frame_new(), flags | PF_PRES));

	image->eax = 0;
	return image;
}

image_t *umap_call(image_t *image) {
	printk("umap(%x)\n", image->eax);

	u32int page = image->eax;
	task_t *t = get_task(curr_pid);

	// Bounds check page address
	if (page > 0xF6000000) {
		image->eax = EPERMIT;
		return image;
	}

	// Check for already freed page
	if ((page_get(&t->map, page) & 0x1) == 0) {
		image->eax = EREPEAT;
		return image;
	}

	// Free page
	frame_free(page_ufmt(page_get(&t->map, page)));
	page_set(&t->map, page, 0x00000000);

	image->eax = 0;
	return image;
}

image_t *rmap_call(image_t *image) {
	u32int src = image->eax;
	u32int dest = image->ebx;
	u32int flags = image->ecx;
	task_t *t = get_task(curr_pid);

	// Bounds check both addresses
	if (src > 0xF6000000 || dest > 0xF6000000) {
		image->eax = EPERMIT;
		return image;
	}

	// Check source
	if ((page_get(&t->map, src) & 0x1) == 0) {
		image->eax = EREPEAT;
		return image;
	}

	// Check destination
	if (page_get(&t->map, dest) & 0x1) {
		image->eax = EREPEAT;
		return image;
	}

	// Move page
	page_set(&t->map, dest, page_get(&t->map, src));
	page_set(&t->map, src, 0x00000000);
	
	image->eax = 0;
	return image;
}
