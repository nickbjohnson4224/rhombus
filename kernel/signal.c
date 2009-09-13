/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

image_t *signal(pid_t targ, uint8_t sig, 
	uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint8_t flags) {
	task_t *dst_t = task_get(targ); 
	task_t *src_t = task_get(curr_pid);

	if (!dst_t) ret(src_t->image, (flags & TF_NOERR) ? targ : ENOTASK);	/* Check target */
	if (flags & TF_BLOCK) src_t->flags |= TF_BLOCK; 					/* Block if set to block */
	task_switch(dst_t); 												/* Switch to target task */

	/* Create new image structure */
	memcpy((uint8_t*)  ((uint32_t) dst_t->image - sizeof(image_t)), dst_t->image, sizeof(image_t));
	dst_t->image = (void*) ((uint32_t) dst_t->image - sizeof(image_t));

	/* Check for task image stack overflows */
	if ((uintptr_t) dst_t->image < SSTACK_BSE + 2 * sizeof(image_t) && !(flags & TF_SUPER))
		signal(targ, S_IMG, 0, 0, 0, 0, TF_SUPER);
	if ((uintptr_t) dst_t->image < SSTACK_BSE + sizeof(image_t))
		return exit_call(dst_t->image);

	/* Set registers to describe signal */
	dst_t->image->eax = arg0;										/* From EAX */
	dst_t->image->ebx = arg1;										/* From EBX */
	dst_t->image->ecx = arg2;										/* From ECX */
	dst_t->image->edx = arg3;										/* From EDX */
	dst_t->image->esi = src_t->pid;									/* Caller PID */
	dst_t->image->edi = sig;										/* Signal number */
	dst_t->image->ebp = (uintptr_t) dst_t->image + sizeof(image_t); /* Points to saved image */
	dst_t->image->eip = dst_t->shandler; 							/* Reentry point */
	return dst_t->image;
}

image_t *sret(image_t *image) {
	task_t *dst_t = task_get(curr_pid);
	task_t *src_t = task_get(image->esi);

	if (src_t && image->eax & TF_UNBLK) src_t->flags &= ~TF_BLOCK;			/* Unblock the caller */
	dst_t->image = (void*) ((uintptr_t) dst_t->image + sizeof(image_t)); 	/* Reset image stack */
	if ((uint32_t) dst_t->image - sizeof(image_t) >= SSTACK_TOP)			/* Bounds check image */
		return exit_call(dst_t->image);

	return dst_t->image;
}
