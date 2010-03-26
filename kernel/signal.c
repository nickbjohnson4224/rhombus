/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <task.h>
#include <int.h>

thread_t *signal(pid_t targ, uint16_t sig, void* grant, uint8_t flags) {
	task_t *dst_t = process_get(targ);
	task_t *src_t = curr_task;
	uintptr_t addr, pflags;

	/* Check target (for existence) */
	if (!dst_t || !dst_t->shandler || (dst_t->sigflags & (1 << sig))) {
		src_t->image->eax = (flags & NOERR) ? targ : ERROR;
		return src_t->image;
	}

	if (grant) {
		addr = (uintptr_t) grant;
		grant = (void*) (page_get(addr) & ~0xFFF);
		pflags = page_get(addr) & 0xFFF;
		page_set(addr, page_fmt(frame_new(), (pflags & PF_MASK) | PF_PRES));
	}

	/* Return 0 in source task */
	src_t->image->eax = 0;

	/* Switch to target task */
	process_switch(dst_t, 0);

	/* Create new image structure below old one */	
	memcpy(&dst_t->image[-1], dst_t->image, sizeof(thread_t));
	dst_t->image = &dst_t->image[-1];

	/* Check for imminent task image stack overflows */
	/* i.e. within two structures of overflow */
	if ((uintptr_t) dst_t->image < SSTACK_BSE + 2 * sizeof(thread_t)) {

		/* Check for a second offense */
		if ((uintptr_t) dst_t->image < SSTACK_BSE + sizeof(thread_t)) {
			return exit(dst_t->image);
		}

		else {
			dst_t->flags |= CTRL_CLEAR;
			dst_t->sigflags = 0xFFFFFFFF;
			signal(targ, SSIG_IMAGE, NULL, CTRL_SUPER);
		}

	}

	/* Make sure handler is unblocked */
	dst_t->flags &= ~CTRL_BLOCK;

	/* Set registers to describe signal */

	/* Granted frame */
	dst_t->image->grant = (uintptr_t) grant;
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

thread_t *sret(thread_t *image) {

	/* Bounds check image */
	if ((uint32_t) curr_task->image >= SSTACK_TOP) {
		return exit(curr_task->image);
	}

	/* Reset thread image stack */
	curr_task->image = &curr_task->image[1];

	return curr_task->image;
}
