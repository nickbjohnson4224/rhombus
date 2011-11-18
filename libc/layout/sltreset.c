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

#include <string.h>

#include <rho/layout.h>
#include <rho/arch.h>
#include <rho/page.h>

void sltreset(void) {
	struct slt32_header *slt_hdr = (void*) SLT_BASE;
	struct slt32_entry  *slt = (void*) SLT_BASE;
	uint32_t i;

	if (phys(slt)) {
		return;
	}

	page_anon(slt, 0x10000, PROT_READ | PROT_WRITE);
	
	slt_hdr->count = 4;
	slt_hdr->mutex = 1;
	slt_hdr->first = 3;

	// region 1 - kernel
	slt[1].type     = SLT_TYPE_KERNEL;
	slt[1].base     = KSPACE;
	slt[1].size     = (uint32_t) -KSPACE;
	slt[1].flags    = 0;
	slt[1].sub_type = 0;
	slt[1].next     = 0;
	strlcpy(slt[1].name, "sys.kernel", 28);
	slt[1].hash = slthash(slt[1].name);

	// region 2 - thread stacks
	slt[2].type     = SLT_TYPE_STACK;
	slt[2].base     = SSPACE;
	slt[2].size     = ESPACE - SSPACE;
	slt[2].flags    = 0;
	slt[2].sub_type = 0;
	slt[2].next     = 1;
	strlcpy(slt[2].name, "sys.stack", 28);
	slt[2].hash = slthash(slt[2].name);

	// region 3 - reserved lower addresses
	slt[3].type     = SLT_TYPE_RESERVED;
	slt[3].base     = 0;
	slt[3].size     = 0x100000;
	slt[3].flags    = 0;
	slt[3].sub_type = 0;
	slt[3].next     = 4;
	strlcpy(slt[3].name, "sys.lower", 28);
	slt[3].hash = slthash(slt[3].name);

	// region 4 - current executable
	slt[4].type     = SLT_TYPE_OBJECT;
	slt[4].base     = 0x100000;
	slt[4].size     = 0x100000;
	slt[4].flags    = SLT_FLAG_CLEANUP;
	slt[4].sub_type = SLT_OBJ_ELF_EXEC;
	slt[4].next     = 2;
	strlcpy(slt[4].name, "sys.exec", 28);
	slt[4].hash = slthash(slt[4].name);

	// free regions
	slt_hdr->free = 5;
	for (i = 5; i < 1024; i++) {
		slt[i].type = SLT_TYPE_NULL;
		slt[i].next = i + 1;
	}
	slt[1023].next = 0;

	slt_hdr->mutex = 0;
}
