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
	thread_t *new_image;
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
	new_image = thread_alloc();
	memcpy(new_image, dst_t->image, sizeof(thread_t));
	new_image->tis  = dst_t->image;
	new_image->proc = dst_t;
	dst_t->image = new_image;

	/* Make sure handler is unblocked */
	dst_t->flags &= ~CTRL_BLOCK;

	/* Granted frame */
	dst_t->image->grant = (uintptr_t) grant;
	dst_t->image->ebx = (uintptr_t) grant;

	/* Caller PID */
	dst_t->image->esi = src_t->pid;

	/* Signal number (for routing) */
	dst_t->image->edi = sig;

	/* Pointer to saved image for debuggers */
	dst_t->image->ebp = (uintptr_t) dst_t->image->tis;

	/* Entry point for signal */
	dst_t->image->eip = dst_t->shandler;

	return dst_t->image;
}

thread_t *sret(thread_t *image) {
	thread_t *old_image;

	/* Reset thread image stack */
	old_image = curr_task->image;
	curr_task->image = old_image->tis;
	thread_free(old_image);

	return curr_task->image;
}
