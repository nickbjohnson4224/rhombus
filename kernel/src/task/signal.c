// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>

image_t *ksignal(u32int task, u32int sig, u32int arg0, u32int arg1, u32int arg2) {
	task_t *t = get_task(task);
	
	// Create new image to return to
	task_switch(task);
	memcpy(t->image, (u8int*) ((u32int) t->image - sizeof(image_t)), sizeof(image_t));
	t->tss_esp = (u32int) t->image; // Will now create images above old image
	t->image = (void*) ((u32int) t->image - sizeof(image_t));

	// Set registers to describe signal
	t->image->eax = 0; // Idle doesn't send signals, so 0 is free for "kernel signals"
	t->image->ebx = arg0;
	t->image->ecx = arg1;
	t->image->edx = arg2;

	// Locate entry point
	t->image->eip = signal_table[sig];

	return t->image;
}

image_t *signal(u32int task, u32int sig, u32int arg0, u32int arg1, u32int arg2, u32int flags) {
	task_t *t, *src_t;
	u32int src = curr_pid;

	// Check existence of target
	src_t = get_task(src);
	t = get_task(task);
	if (t->magic != 0x4224) {
		src_t->image->eax = ENOTASK;
		return src_t->image;
	}

	// Block if set to block
	if (flags & TF_BLOCK) src_t->flags |= TF_BLOCK;
	
	// Create new image to return to
	task_switch(task);
	memcpy((u8int*) ((u32int) t->image - sizeof(image_t)), t->image, sizeof(image_t));
	t->tss_esp = (u32int) t->image; // Will now create images above old image
	t->image = (void*) ((u32int) t->image - sizeof(image_t));

	// Set registers to describe signal
	t->image->eax = src;
	t->image->ebx = arg0;
	t->image->ecx = arg1;
	t->image->edx = arg2;

	// Locate entry point
	t->image->eip = signal_table[sig];

	return t->image;
}

image_t *sret() {
	task_t *t;

	// Reset image stack
	t->image = (void*) ((u32int) t->image + sizeof(image_t));
	t->tss_esp = (u32int) t->image + sizeof(image_t);

	// Bounds check image
	// If this is false, we really should kill the process...
	if ((u32int) t->image >= 0xF3FFEFFC - sizeof(image_t)) 
		t->image = (void*) (0xF3FFEFFC - sizeof(image_t));

	return t->image;
}
