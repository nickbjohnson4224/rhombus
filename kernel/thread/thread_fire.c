/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <task.h>
#include <int.h>

/****************************************************************************
 * thread_fire
 *
 * Sends a signal to the process with pid targ of the type sig with the
 * granted page at current virtual address grant. If the target process has 
 * the CTRL_QUEUE flag set, the signal is added to that process' mailbox. 
 * Otherwise, a new thread is created in the target process to handle the 
 * incoming signal, and that thread is switched to.
 *
 * The granted page in the current process is replaced with a page with
 * undefined contents and the same permissions as the granted page.
 *
 * Returns a runnable and active thread that may or may not be the thread
 * passed as image.
 */

thread_t *thread_fire(thread_t *image, uint16_t targ, uint16_t sig, uintptr_t grant){
	process_t *p_targ;
	thread_t *new_image;
	uintptr_t addr;

	p_targ = process_get(targ);

	if (grant) {
		addr   = grant;
		grant  = page_get(grant);
		page_set(addr, page_fmt(frame_new(), (grant & 0xFFF) | PF_PRES));
		grant &= ~0xFFF;
	}


	if (p_targ->flags & CTRL_QUEUE) {
		/* queue signal */

		/* XXX - please implement */
		return image;

	}
	else {
		/* send signal */

		if (p_targ->shandler == 0) {
			return image;
		}

		new_image = thread_alloc();

		new_image->ds      = 0x23;
		new_image->cs      = 0x1B;
		new_image->ss      = 0x23;
		new_image->eflags  = p_targ->image->eflags;
		new_image->useresp = p_targ->image->useresp;
		new_image->tis     = p_targ->image;
		new_image->proc    = p_targ;
		new_image->grant   = grant;
		new_image->ebx     = grant;
		new_image->source  = image->proc->pid;
		new_image->esi     = image->proc->pid;
		new_image->signal  = sig;
		new_image->edi     = sig;
		new_image->eip     = p_targ->shandler;

		p_targ->image  = new_image;
		p_targ->flags &= ~CTRL_BLOCK;
	
		return thread_switch(image, new_image);
	}
}
