/* Copyright 2009, 2010 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

image_t *signal(pid_t targ, uint16_t sig, void* grant, uint8_t flags) {
	task_t *dst_t = task_get(targ);
	task_t *src_t = curr_task;
	uintptr_t addr;

	/* Check target (for existence) */
	if (!dst_t || !dst_t->shandler) {
		ret(src_t->image, (flags & NOERR) ? targ : ERROR);
	}

	if ((dst_t->flags & CTRL_CLEAR) && (flags & CTRL_SUPER) == 0) {
		ret(src_t->image, (flags & NOERR) ? targ : ERROR);
	}

	/* Get frame of grant */
	if (grant) {
		addr = (uintptr_t) grant;
		grant = (void*) (page_get(addr) & ~0xFFF);
		page_set(addr, 0);
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
		if ((uintptr_t) dst_t->image < SSTACK_BSE + sizeof(image_t)) {
			return exit(dst_t->image);
		}

		else {
			dst_t->flags |= CTRL_CLEAR;
			signal(targ, SSIG_IMAGE, NULL, CTRL_SUPER);
		}

	}

	/* Save grant and flags */
	dst_t->image[1].grant = dst_t->grant;
	dst_t->image[1].flags = dst_t->flags;

	/* Modify flags */
	if (dst_t->flags & CTRL_CBLOCK) {
		dst_t->flags |= CTRL_BLOCK;
	}
	else {
		dst_t->flags &= ~CTRL_BLOCK;
	}

	if (dst_t->flags & CTRL_CCLEAR) {
		dst_t->flags ^= CTRL_CLEAR;
	}

	/* Set registers to describe signal */

	/* Granted frame */
	dst_t->grant = (uintptr_t) grant;
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
	uint32_t flags;

	/* Bounds check image */
	if ((uint32_t) curr_task->image >= SSTACK_TOP) {
		return exit(curr_task->image);
	}

	/* Reset task image stack */
	curr_task->image = &curr_task->image[1];

	/* Reload saved grant */
	curr_task->grant = curr_task->image->grant;

	/* Reload/modify flags */
	flags = curr_task->flags;
	curr_task->flags &= ~(CTRL_BLOCK | CTRL_CLEAR);
	curr_task->flags |= curr_task->image->flags & (CTRL_BLOCK | CTRL_CLEAR);

	if (flags & CTRL_DBLOCK) {
		curr_task->flags |= CTRL_BLOCK;
	}

	if (flags & CTRL_DCLEAR) {
		curr_task->flags ^= CTRL_CLEAR;
	}

	return curr_task->image;
}
