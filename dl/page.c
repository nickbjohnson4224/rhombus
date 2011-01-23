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

#include "dl.h"

int dl_page(void *addr, size_t length, int prot, int source, uintptr_t off) {

	if ((uintptr_t) addr % PAGESZ) {
		length += (uintptr_t) addr % PAGESZ;
		addr    = (void*) ((uintptr_t) addr - ((uintptr_t) addr % PAGESZ));
	}

	if (length % PAGESZ) {
		length = (length / PAGESZ) + 1;
	}
	else {
		length = length / PAGESZ;
	}

	return _dl_page(addr, length, prot, source, off);
}

int dl_page_free(void *addr, size_t length) {
	return dl_page(addr, length, 0, PAGE_NULL, 0);
}

int dl_page_anon(void *addr, size_t length, int prot) {
	return dl_page(addr, length, prot, PAGE_ANON, 0);
}

int dl_page_self(void *addrs, void *addrd, size_t length) {
	return dl_page(addrd, length, 0, PAGE_SELF, (uintptr_t) addrs);
}

int dl_page_prot(void *addr, size_t length, int prot) {
	return dl_page(addr, length, prot, PAGE_PROT, 0);
}
