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
 * syscall_keys (int 0x54)
 *
 * EBX: operation
 * ECX: value
 * EDX: target
 * EDI: resource PID
 * ESI: resource index
 *
 * KEY_CHECK - 0:
 *   Check the key value of a given key from <target>. If <target> is zero,
 *   the current process is used instead.
 *
 * KEY_GRANT - 1:
 *   Create key with given value and grant to target. This key is a new key, 
 *   and must have the same resource PID as the caller.
 *
 * KEY_SHARE - 2:
 *   NOT IMPLEMENTED
 *
 * KEY_DELETE - 3:
 *   
 *
 * KEY_RECALL - 4:
 *   NOT IMPLEMENTED
 *
 * KEY_COUNT - 5:
 *   Return the number of active (i.e. nonzero) keys in the system to the 
 *   given resource.
 */

struct thread *syscall_keys(struct thread *image) {
	struct process *proc;
	uint64_t resource;
	uint32_t keyvalue;
	uint32_t target;
	uint32_t operation;

	operation = image->ebx;
	keyvalue  = image->ecx;
	target    = (image->edx) ? image->edx : image->proc->pid;
	resource  = ((uint64_t) image->edi << 32) | (uint64_t) image->esi;

	switch (operation) {
	case 0: /* KEY_CHECK */
		proc = process_get(target);

		if (proc) {
			image->eax = key_get(proc, resource);
		}
		else {
			image->eax = 0;
		}
		break;
	case 1: /* KEY_GRANT */
		proc = process_get(target);

		if (image->edi != image->proc->pid) {
			image->eax = -1;
		}
		else {
			key_set(proc, resource, keyvalue);
			image->eax = 0;
		}
		break;
	case 2: /* KEY_SHARE */
		proc = process_get(target);

		key_set(proc, resource, key_get(image->proc, resource));
		image->eax = 0;
		break;
	case 3: /* KEY_DELETE */
		
		key_set(image->proc, resource, 0);
		image->eax = 0;
		break;
	case 4: /* KEY_RECALL */

		// XXX - not implemented

		image->eax = 0;
		break;
	}
	
	return image;
}
