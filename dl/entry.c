/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <string.h>

#include <rho/layout.h>
#include <rho/arch.h>

#include "dl.h"

__attribute__ ((section (".head")))
int dl_entry(struct dl_list *list, size_t count) {
	struct dl_list *exec_entry;
	struct slt32_header *slt_hdr;
	struct slt32_entry *slt;
	struct elf32_ehdr *exec;
	size_t size;
	void  *entry;
	size_t i;

	dl__init();

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

	/*** POINT OF MAYBE RETURNING IF YOU, y'know, have to... ***/

	/* copy executable high */
	size = exec_entry->size;
	exec = (void*) sltalloc("dl.exec", size);

	if ((uintptr_t) exec_entry->base % PAGESZ) {
		/* not aligned, copy */
		page_anon(exec, size, PROT_READ | PROT_WRITE);
		memcpy(exec, exec_entry->base, size);
	}
	else {
		/* aligned, use paging */
		page_self(exec_entry->base, exec, size);
	}

	/*** POINT OF NO RETURN ***/

	/* clear lower memory */
	slt     = (void*) SLT_BASE;
	slt_hdr = (void*) SLT_BASE;

	for (i = slt_hdr->first; i; i = slt[i].next) {
		if (slt[i].flags & SLT_FLAG_CLEANUP) {
			page_free((void*) slt[i].base, slt[i].size);
		}
	}

	/* load executable */
	dl_elf_load(exec);
	entry = (void*) exec->e_entry;

	/* remove executable image */
	page_free(exec, size);
	sltfree_name("dl.exec");

	/* reset event handler */
	_when(0);

	/* enter executable */
	dl_enter(entry);

	return 1;
}
