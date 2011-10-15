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
 * EBX: operation
 * ECX:EDX: rp of A
 * ESI:EDI: rp of B
 *
 * operations:
 *
 * RTAB_OPEN - 0
 *
 *   Establish a connection between A and B.
 *   
 * RTAB_CLOSE - 1
 *
 *   Close the connection between A and B.
 */

struct thread *syscall_rtab(struct thread *image) {
	struct process *proc;
	uint64_t rp_a;
	uint64_t rp_b;

	rp_a = (uint64_t) image->ecx | ((uint64_t) image->edx  << 32);
	rp_b = (uint64_t) image->esi | ((uint64_t) image->edi  << 32);

	debug_printf("RTAB %d %d %d %d %d\n", image->ebx, image->ecx, image->edx, image->esi, image->edi);

	switch (image->edx) {
	case 0: /* RTAB_OPEN */

		if (image->esi != image->proc->pid) {
			// B must belong to the process
			image->eax = 1;
			return image;
		}

		proc = process_get(image->ecx);

		if (!proc) {
			image->eax = 1;
			return image;
		}

		rtab_open(proc, rp_a, rp_b);
		
		image->eax = 0;
		return image;

	case 1: /* RTAB_CLOSE */

		if (image->ecx != image->proc->pid) {
			// A must belong to the process
			image->eax = 1;
			return image;
		}

		rtab_close(image->proc, rp_a, rp_b);
		
		image->eax = 0;
		return image;

	default:

		image->eax = 1;
		return image;

	}
}
