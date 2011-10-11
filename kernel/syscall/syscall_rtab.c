/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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
#include <process.h>
#include <debug.h>

/*****************************************************************************
 * syscall_rtab (int 0x54)
 *
 * EBX: index (of rp)
 * ECX: pid (of rp) or target
 * EDX: operation
 *
 * operations:
 *
 * RTAB_GRANT - 0
 *   
 *   Grant the current process' robject with index <index> to PID <target>.
 *
 * RTAB_CLOSE - 1
 *
 *   Delete the robject <pid:index> from the current process' robject table.
 *
 * RTAB_COUNT - 2
 *
 *   Return the reference count of the robject <pid:index>.
 */

struct thread *syscall_rtab(struct thread *image) {
	struct process *proc;
	uint32_t target;
	uint32_t index;
	uint64_t rp;

	index  = image->ebx;
	target = image->ecx;
	rp = (uint64_t) index | ((uint64_t) target << 32);

	switch (image->edx) {
	case 0: /* RTAB_GRANT */

		rp = (uint64_t) index | ((uint64_t) image->proc->pid << 32);
		proc = process_get(target);

		if (!proc) {
			image->eax = 1;
			return image;
		}

		rtab_grant(proc, rp);
		
		image->eax = 0;
		return image;

	case 1: /* RTAB_CLOSE */

		rtab_close(image->proc, rp);
		
		image->eax = 0;
		return image;

	case 2: /* RTAB_COUNT */

		image->eax = rtab_count(rp);
		return image;
	
	default:

		image->eax = 1;
		return image;

	}
}
