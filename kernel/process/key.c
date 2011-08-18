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

void key_set(struct process *proc, uint64_t resource, uint32_t keyvalue) {
	struct key *old_keytable;
	uint32_t i;

	for (i = 0; i < proc->keycount; i++) {
		if (proc->keytable[i].resource == resource) {
			proc->keytable[i].keyvalue = keyvalue;
			return;
		}
	}
	
	old_keytable = proc->keytable;
	proc->keytable = heap_alloc(sizeof(struct key) * (proc->keycount + 16));

	for (i = 0; i < proc->keycount; i++) {
		proc->keytable[i].resource = old_keytable[i].resource;
		proc->keytable[i].keyvalue = old_keytable[i].keyvalue;
	}

	for (; i < proc->keycount + 16; i++) {
		proc->keytable[i].keyvalue = 0;
	}

	proc->keytable[proc->keycount].resource = resource;
	proc->keytable[proc->keycount].keyvalue = keyvalue;

	heap_free(old_keytable, proc->keycount * sizeof(struct key));
	proc->keycount += 16;
}

uint32_t key_get(struct process *proc, uint64_t resource) {
	uint32_t i;

	for (i = 0; i < proc->keycount; i++) {
		if (proc->keytable[i].resource == resource) {
			return proc->keytable[i].keyvalue;
		}
	}

	return 0;
}
