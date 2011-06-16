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
 * syscall_name (int 0x53)
 *
 * EAX: name (bytes 0-3)
 * EBX: name (bytes 4-7)
 * ECX: name (bytes 7-B)
 * EDX: name (bytes C-F)
 * ESI: operation
 * EDI: pid
 *
 * Reads or modifies the process name of a process. If <operation> is zero,
 * the contents of EAX:EBX:ECX:EDX are set to the process name of the process
 * with PID <pid>. Otherwise, the process name of the process with PID <pid>
 * is set to EAX:EBX:ECX:EDX. Processes may only set their own names.
 */

struct thread *syscall_name(struct thread *image) {
	pid_t pid = image->edi;
	struct process *proc;

	if (image->esi) {
		
		/* check process ID before setting */
		if (pid != image->proc->pid) {
			image->eax = 0;
			return image;
		}

		/* copy name (assuming pid = getpid()) */
		*((uint32_t *) &image->proc->name[0x00]) = image->eax;
		*((uint32_t *) &image->proc->name[0x04]) = image->ebx;
		*((uint32_t *) &image->proc->name[0x08]) = image->ecx;
		*((uint32_t *) &image->proc->name[0x0C]) = image->edx;

		return image;
	}
	else {
		proc = process_get(pid);

		if (!proc) {
			image->eax = 0;
			return image;
		}

		/* copy name */
		image->eax = *((uint32_t *) &proc->name[0x00]);
		image->ebx = *((uint32_t *) &proc->name[0x04]);
		image->ecx = *((uint32_t *) &proc->name[0x08]);
		image->edx = *((uint32_t *) &proc->name[0x0C]);

		return image;
	}
}
