/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdint.h>
#include <rho/arch.h>
#include "dl.h"

__attribute__ ((section (".head")))
int dl_entry(struct dl_list *list, size_t count) {
	struct dl_list *exec_entry;
	struct elf32_ehdr *exec;
	size_t size;
	void  *entry;
	size_t i;
	
	/* check for null list */
	if (list == NULL) {
		return 1;
	}

	/* find entry for executable */
	for (exec_entry = NULL, i = 0; i < count; i++) {
		if (list[i].type == DL_EXEC) {
			exec_entry = &list[i];
			break;
		}
	}

	/* reject if no executable found */
	if (!exec_entry) {
		return 1;
	}

	/* check executable */
	if (dl_elf_check(exec_entry->base)) {
		return 1;
	}

	/* copy executable high */
	size = exec_entry->size;
	exec = (void*) DL_HEAP;

	if ((uintptr_t) exec_entry->base % PAGESZ) {
		/* not aligned, copy */
		dl_page_anon(exec, size, PROT_READ | PROT_WRITE);
		dl_memcpy(exec, exec_entry->base, size);
		dl_page_prot(exec, size, PROT_READ);
	}
	else {
		/* aligned, use paging */
		dl_page_self(exec_entry->base, exec, size);
		dl_page_prot(exec, size, PROT_READ);
	}

	/* clear lower memory */
	dl_page_free(NULL, 0x80000000);

	/* load executable */
	dl_elf_load(exec);
	entry = (void*) exec->e_entry;

	/* remove executable image */
	dl_page_free(exec, size);

	/* reset event handler */
	_dl_when(0);

	/* enter executable */
	dl_enter(entry);

	return 1;
}
