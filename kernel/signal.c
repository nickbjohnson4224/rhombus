/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

image_t *signal(pid_t targ, uint8_t sig, 
	uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint8_t flags) {
	task_t *t, *src_t;
	pid_t caller = curr_pid;

	/* Check existence of target */
	t = task_get(targ);
	src_t = task_get(caller);
	if (!t) {
		if (flags & TF_NOERR) return src_t->image;
		else ret(src_t->image, ENOTASK);
	}

	/* Switch to target task */
	if (t->pid != curr_pid) task_switch(t);

	/* Block if set to block */
	if (flags & TF_BLOCK) src_t->flags |= TF_BLOCK;
	if (flags & TF_UNBLK) t->flags &= ~TF_BLOCK;

	/* Save caller */
	t->image->caller = t->caller;
	t->caller = caller;

	/* Create new image structure */
	memcpy((uint8_t*)  ((uint32_t) t->image - sizeof(image_t)), t->image, sizeof(image_t));
	t->image = (void*) ((uint32_t) t->image - sizeof(image_t));

	/* Check for task image stack overflows */
	if ((uint32_t) t->image < SSTACK_BSE + 2 * sizeof(image_t) && !(flags & TF_SUPER)) 
		signal(targ, S_IMG, 0, 0, 0, 0, TF_SUPER);
	if ((uint32_t) t->image < SSTACK_BSE + sizeof(image_t))
		panic("task image stack overflow"); /* XXX - change to process exit */

	/* Set registers to describe signal */
	t->image->eax = arg0;
	t->image->ebx = arg1;
	t->image->ecx = arg2;
	t->image->edx = arg3;
	t->image->esi = caller;
	t->image->edi = sig;
	t->image->ebp = t->tss_esp; /* Points to saved parent image for analysis */

	/* Set reentry point */
	t->image->eip = t->shandler;

	return t->image;
}

image_t *sret(image_t *image) {
	task_t *t, *src_t;

	/* Unblock the caller */
	t = task_get(curr_pid);
	src_t = task_get(t->caller);
	if (src_t) {
		if (image->eax & TF_UNBLK) {
			src_t->flags &= ~TF_BLOCK;
		}
	}

	/* Reset image stack */
	t->image = (void*) ((uintptr_t) t->image + sizeof(image_t));
	t->caller = t->image->caller;

	/* Bounds check image */
	if ((uint32_t) t->image - sizeof(image_t) >= SSTACK_TOP) 
		panic("task image stack underflow");

	return t->image;
}
