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

#include <process.h>
#include <thread.h>
#include <debug.h>
#include <space.h>
#include <types.h>
#include <ipc.h>

/*****************************************************************************
 * rtab_free
 *
 * Delete all resource table entries for the process <proc>.
 */

void rtab_free(struct process *proc) {
	uint32_t i;

	// close all entries
	for (i = 0; i < proc->rtab_count; i++) {
		if (proc->rtab[i].a) {
			rtab_close(proc, proc->rtab[i].a, proc->rtab[i].b);
		}
	}

	if (proc->rtab) heap_free(proc->rtab, proc->rtab_count * sizeof(struct rtab));
	proc->rtab = NULL;
	proc->rtab_count = 0;
}

/*****************************************************************************
 * rtab_close
 *
 * Delete the resource table entry for the connection <a>,<b> in the process 
 * <proc>. A notification is sent to <b> on port PORT_CLOSE if successful.
 */

void rtab_close(struct process *proc, uint64_t a, uint64_t b) {
	uint32_t i;

	// find slot
	for (i = 0; i < proc->rtab_count; i++) {
		
		if (proc->rtab[i].a == a || proc->rtab[i].b == b) {

			// found slot
			thread_sendv(b, a, PORT_CLOSE);

			proc->rtab[i].a = 0;
			proc->rtab[i].b = 0;
			return;
		}
	}

	// could not find slot
	return;
}

/*****************************************************************************
 * rtab_open
 *
 * Add a resource table entry for the connection <a>,<b> to the process 
 * <proc>.
 */

void rtab_open(struct process *proc, uint64_t a, uint64_t b) {
	uint32_t count;
	struct rtab *temp;
	struct rtab *temp2;
	uint32_t i;

	// find empty slot for entry
	for (i = 0; i < proc->rtab_count; i++) {

		if (proc->rtab[i].a == a || proc->rtab[i].b == b) {
			// already contains this connection
			return;
		}

		if (proc->rtab[i].a == 0) {
			// found suitable entry
			proc->rtab[i].a = a;
			proc->rtab[i].b = b;
			return;
		}
	}

	// could not find slot; reallocate table
	count = proc->rtab_count + 16;
	temp = heap_alloc(count * sizeof(struct rtab));

	// copy contents to new table
	for (i = 0; i < proc->rtab_count; i++) {
		temp[i].a = proc->rtab[i].a;
		temp[i].b = proc->rtab[i].b;
	}

	// clear remainder of table
	for (; i < count; i++) {
		temp[i].a = 0;
		temp[i].b = 0;
	}

	// free old table
	temp2 = proc->rtab;
	proc->rtab = temp;
	if (temp2) heap_free(temp2, proc->rtab_count * sizeof(struct rtab));
	proc->rtab_count = count;

	// add new entry
	rtab_open(proc, a, b);
}
