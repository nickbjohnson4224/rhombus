/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <interrupt.h>
#include <space.h>
#include <ipc.h>

/*****************************************************************************
 * syscall_gvpr (int 0x44)
 *
 * ECX: field
 * EDX: addr
 *
 * Gets the value of <field> in the current thread's virtual packet register
 * and returns it in EAX, unless the field is zero, in which case it maps the
 * packet frame to <addr>. Returns zero on failure.
 */

struct thread *syscall_gvpr(struct thread *image) {
   uintptr_t addr = image->edx;
   uint32_t field = image->ecx;

   if (field == 0) {
	   if (!image->packet || !image->packet->frame) {
		   image->eax = 0;
	   }

	   else if (addr & 0xFFF || addr + PAGESZ >= KSPACE) {
		   image->eax = -1;
	   }

	   else {
		   if (page_get(addr) & PF_PRES) {
			   frame_free(page_get(addr));
		   }
	   
		   page_set(addr, page_fmt(image->packet->frame, PF_PRES|PF_USER|PF_RW));

		   image->eax = 1;
	   }
   }

   else if (field == 1) {
	   if (!image->packet) {
		   image->eax = 0;
	   }
	   else {
		   image->eax = image->packet->source;
	   }
   }

   else if (field == 2) {
	   if (!image->packet || !image->packet->frame) {
		   image->eax = 0;
	   }
	   else {
		   image->eax = PAGESZ;
	   }
   }

   else if (field == 3) {
	   if (!image->packet) {
		   image->eax = 0;
	   }
	   else {
		   image->eax = image->packet->port;
	   }
   }

   return image;
}
