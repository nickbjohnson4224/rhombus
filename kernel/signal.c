/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

image_t *signal(pid_t targ, uint8_t sig, 
	uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint8_t flags) {

	task_t *dst_t = task_get(targ); 
	task_t *src_t = task_get(curr_pid);
	pid_t caller = curr_pid;

	/* Check existence of target */
	if (!dst_t) {
		if (flags & TF_NOERR) return src_t->image;
		else ret(src_t->image, ENOTASK);
	}

	/* Switch to target task */
	task_switch(dst_t);

	/* Block if set to block */
	if (flags & TF_BLOCK) src_t->flags |= TF_BLOCK;

	/* Create new image structure */
	memcpy((uint8_t*)  ((uint32_t) dst_t->image - sizeof(image_t)), dst_t->image, sizeof(image_t));
	dst_t->image = (void*) ((uint32_t) dst_t->image - sizeof(image_t));

	/* Check for task image stack overflows */
	if ((uintptr_t) dst_t->image < SSTACK_BSE + 2 * sizeof(image_t) && !(flags & TF_SUPER)) 
		signal(targ, S_IMG, 0, 0, 0, 0, TF_SUPER);
	if ((uintptr_t) dst_t->image < SSTACK_BSE + sizeof(image_t))
		return exit_call(dst_t->image);

	/* Set registers to describe signal */
	dst_t->image->eax = arg0;
	dst_t->image->ebx = arg1;
	dst_t->image->ecx = arg2;
	dst_t->image->edx = arg3;
	dst_t->image->esi = caller;
	dst_t->image->edi = sig;
	dst_t->image->ebp = (uintptr_t) dst_t->image + sizeof(image_t); /* Points to saved image */
	dst_t->image->eip = dst_t->shandler; /* Reentry point */

	return dst_t->image;
}

image_t *sret(image_t *image) {
	task_t *dst_t = task_get(curr_pid);
	task_t *src_t = task_get(image->esi);

	/* Unblock the caller */
	if (src_t && image->eax & TF_UNBLK) src_t->flags &= ~TF_BLOCK;

	/* Reset image stack */
	dst_t->image = (void*) ((uintptr_t) dst_t->image + sizeof(image_t));

	/* Bounds check image */
	if ((uint32_t) dst_t->image - sizeof(image_t) >= SSTACK_TOP)
		return exit_call(dst_t->image);

	return dst_t->image;
}
