/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

image_t *signal(pid_t targ, uint8_t sig, uint32_t args[4], uint8_t flags) {
	task_t *dst_t = task_get(targ);
	task_t *src_t = curr_task;
	void *next_image;

	printk("signal: %d -> %d sig %d\n", curr_pid, targ, sig);

	/* Check target (for existence) */
	if (!dst_t || !dst_t->shandler || (dst_t->flags & TF_SBLOK)) {
		ret(src_t->image, (flags & TF_NOERR) ? targ : ERROR);
	}

	/* Switch to target task */
	task_switch(dst_t);

	/* Create new image structure */
	next_image = (void*) ((uint32_t) dst_t->image - sizeof(image_t));
	memcpy(next_image, dst_t->image, sizeof(image_t));
	dst_t->image = next_image;

	/* Check for imminent task image stack overflows */
	/* i.e. within two structures of overflow */
	if ((uintptr_t) dst_t->image < SSTACK_BSE + 2 * sizeof(image_t)) {

		/* Check for a second offense */
		if (flags & TF_SUPER) {
			return exit_call(dst_t->image);
		}

		else {
			args[0] = (uint32_t) src_t->pid;
			signal(targ, S_IMG, args, TF_SUPER);
		}

	}

	/* Set registers to describe signal */

	/* Match arguments to first four registers */
	dst_t->image->eax = args[0];
	dst_t->image->ebx = args[1];
	dst_t->image->ecx = args[2];
	dst_t->image->edx = args[3];

	/* Caller PID */
	dst_t->image->esi = src_t->pid;

	/* Signal number (for routing) */
	dst_t->image->edi = sig;

	/* Pointer to saved image for debuggers */
	dst_t->image->ebp = (uintptr_t) dst_t->image + sizeof(image_t);

	/* Entry point for signal */
	dst_t->image->eip = dst_t->shandler;

	return dst_t->image;
}

image_t *sret(image_t *image) {
	task_t *dst_t = curr_task;

	/* Bounds check image */
	if ((uint32_t) dst_t->image >= SSTACK_TOP) {
		return exit_call(dst_t->image);
	}

	/* Reset task image stack */
	dst_t->image = (void*) ((uintptr_t) dst_t->image + sizeof(image_t));

	return dst_t->image;
}
