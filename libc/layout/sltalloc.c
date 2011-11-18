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

void *sltalloc(const char *name, size_t size) {
	struct slt32_entry  *slt = (void*) SLT_BASE;
	struct slt32_header *slt_hdr = (void*) SLT_BASE;
	uint32_t base;
	uint32_t i, t;

	size = SLT_ASLR_PADDING + ((size & 0xFFF) ? (size & ~0xFFF) + 0x1000 : size);
	base = 0;

	// XXX - lock
	
	i = slt_hdr->first;

	while (slt[i].next) {
		if (slt[i].base + slt[i].size + size <= slt[slt[i].next].base) {
			base = slt[i].base + slt[i].size;
			t = slt_hdr->free;
			slt_hdr->free = slt[t].next;
			slt[t].next = slt[i].next;
			slt[i].next = t;
			
			slt[t].base     = base;
			slt[t].size     = size;
			slt[t].type     = SLT_TYPE_NULL;
			slt[t].flags    = 0;
			slt[t].aslr_off = 0x1000; // XXX - implement ASLR
			slt[t].hash     = slthash(name);
			strlcpy(slt[t].name, name, 28);

			slt_hdr->count++;
			break;
		}
		i = slt[i].next;
	}

	// XXX - unlock
	
	return (void*) base;
}