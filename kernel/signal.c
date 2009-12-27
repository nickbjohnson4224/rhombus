/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

image_t *signal(pid_t targ, uint16_t sig, void* grant, uint8_t flags) {
	task_t *dst_t = task_get(targ);
	task_t *src_t = curr_task;

	/* Check target (for existence) */
	if (!dst_t || !dst_t->shandler) {
		ret(src_t->image, (flags & TF_NOERR) ? targ : ERROR);
	}

	if ((dst_t->flags & TF_SBLOK) && (flags & TF_SUPER) == 0) {
		ret(src_t->image, (flags & TF_NOERR) ? targ : ERROR);
	}

	/* Switch to target task */
	task_switch(dst_t);

	/* Create new image structure below old one */	
	memcpy(&dst_t->image[-1], dst_t->image, sizeof(image_t));
	dst_t->image = &dst_t->image[-1];

	/* Check for imminent task image stack overflows */
	/* i.e. within two structures of overflow */
	if ((uintptr_t) dst_t->image < SSTACK_BSE + 2 * sizeof(image_t)) {

		/* Check for a second offense */
		if (flags & TF_SUPER) {
			return exit(dst_t->image);
		}

		else {
			dst_t->flags |= TF_SBLOK;
			signal(targ, S_IMG, NULL, TF_SUPER);
		}

	}

	/* Set registers to describe signal */

	/* Granted frame */
	dst_t->image->ebx = (uintptr_t) grant;

	/* Caller PID */
	dst_t->image->esi = src_t->pid;

	/* Signal number (for routing) */
	dst_t->image->edi = sig;

	/* Pointer to saved image for debuggers */
	dst_t->image->ebp = (uintptr_t) &dst_t->image[1];

	/* Entry point for signal */
	dst_t->image->eip = dst_t->shandler;

	return dst_t->image;
}

image_t *sret(image_t *image) {

	/* Bounds check image */
	if ((uint32_t) curr_task->image >= SSTACK_TOP) {
		return exit(curr_task->image);
	}

	/* Reset task image stack */
	curr_task->image = &curr_task->image[1];

	return curr_task->image;
}
