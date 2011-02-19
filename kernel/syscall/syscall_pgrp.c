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

/*****************************************************************************
 * syscall_pgrp (int 0x50)
 *
 * ECX: group
 * EDX: action
 * EDI: target
 *
 * Manages process groups, depending on the value of <action>.
 *
 * If <action> is 0, the process <target>'s process group is set to <group>. 
 * If the action is successful, 0 is returned; otherwise, 1 is returned.
 *
 * If <action> is 1, a new process group is constructed with the current
 * process' process group as its parent, and the process is assigned to the
 * new process group. The GID of the new process group is returned on success,
 * and 0 is returned on failure.
 */

struct thread *syscall_pgrp(struct thread *image) {
	gid_t pgroup;
	pid_t target = image->edi;
	
	switch (image->edx) {
	case 0:
		image->eax = pgroup_move(image->ecx, image->proc);
		break;
	case 1:
		pgroup = pgroup_alloc();

		if (!pgroup || !process_get(target)) {
			image->eax = 0;
		}
		else {
			if (pgroup_move(pgroup, process_get(target))) {
				image->eax = 0;
			}
			else {
				image->eax = pgroup;
			}
		}
		break;
	}

	return image;
}
