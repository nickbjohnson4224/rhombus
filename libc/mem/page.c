/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <page.h>
#include <abi.h>

int page(void *addr, size_t length, int prot, int source, uintptr_t off) {

	if ((uintptr_t) addr & 0xFFF) {
		length += (uintptr_t) addr & 0xFFF;
		addr    = (void*) ((uintptr_t) addr - ((uintptr_t) addr & 0xFFF));
	}

	if (length & 0xFFF) {
		length = (length / PAGESZ) + 1;
	}
	else {
		length = length / PAGESZ;
	}

	return _page((uintptr_t) addr, length, prot, source, off);
}

int page_free(void *addr, size_t length) {
	return page(addr, length, 0, PAGE_NULL, 0);
}

int page_anon(void *addr, size_t length, int prot) {
	return page(addr, length, prot, PAGE_ANON, 0);
}

int page_pack(void *addr, size_t length, int prot) {
	return page(addr, length, prot, PAGE_PACK, 0);
}

int page_phys(void *addr, size_t length, int prot, uintptr_t base) {
	return page(addr, length, prot, PAGE_PHYS, base);
}
