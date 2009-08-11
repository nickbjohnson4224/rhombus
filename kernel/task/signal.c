// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>

u32int *signal_table = (void*) SIG_TBL; // User signal table
u32int *sigovr_table = (void*) SOV_TBL; // Libsys signal override table

image_t *signal(u16int task, u8int sig, 
	u32int arg0, u32int arg1, u32int arg2, u32int arg3, u8int flags) {
	task_t *t, *src_t;
	u32int caller = curr_pid;

	// Check existence of target
	t = get_task(task);
	src_t = get_task(caller);
	if (t->magic != 0x4224) {
		if (flags & TF_NOERR) return src_t->image;
		else ret(src_t->image, ENOTASK);
	}

	// Check permissions
	if (t->user.id != src_t->user.id && t->user.ring >= src_t->user.ring) {
		if (flags & TF_NOERR) return src_t->image;
		else ret(src_t->image, EPERMIT);
	}

	// Switch to target task
	tss_set_esp(t->tss_esp);
	if (t->pid != curr_pid) task_switch(t);

	// Check existence of signal handler
	if (!sigovr_table[sig] && !signal_table[sig]) {
		task_switch(get_task(caller));
		if (flags & TF_NOERR) return src_t->image;
		else ret(src_t->image, ENOSIG);
	}

	// Block if set to block
	if (flags & TF_BLOCK) src_t->flags |= TF_BLOCK;
	if (flags & TF_UNBLK) t->flags &= ~TF_BLOCK;

	// Save caller
	t->image->caller = t->caller;
	t->caller = caller;

	// Create new image structure
	memcpy((u8int*) ((u32int) t->image - sizeof(image_t)), t->image, sizeof(image_t));
	t->tss_esp = (u32int) t->image; // Will now create image below old image
	t->image = (void*) ((u32int) t->image - sizeof(image_t));
	if ((u32int) t->image < SSTACK_BSE + sizeof(image_t)) {
		printk("%x\n", t->image);
		panic("task state stack overflow");	
	}

	// Set registers to describe signal
	t->image->eax = arg0;
	t->image->ebx = arg1;
	t->image->ecx = arg2;
	t->image->edx = arg3;
	t->image->esi = caller;
	t->image->edi = sig;

	// Set reentry point
	t->image->eip = (sigovr_table[sig]) ? sigovr_table[sig] : signal_table[sig];
	tss_set_esp(t->tss_esp);
	return t->image;
}

image_t *sret(image_t *image) {
	task_t *t, *src_t;

	// Unblock the caller
	t = get_task(curr_pid);
	src_t = get_task(t->caller);
	if (image->eax & TF_UNBLK) {
//		printk("SRET: unblocking %d\n", t->caller);
		src_t->flags &= ~TF_BLOCK;
	}

	// Reset image stack
	t->image = (void*) t->tss_esp;
	t->tss_esp += sizeof(image_t); 
	t->caller = t->image->caller;

	// Bounds check image
	if ((u32int) t->tss_esp >= SSTACK_TOP) 
		panic("task state stack underflow");

	tss_set_esp(t->tss_esp);
	return t->image;
}
