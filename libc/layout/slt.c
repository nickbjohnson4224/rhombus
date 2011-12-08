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

#include <rho/layout.h>
#include <rho/exec.h>

void *sltalloc(const char *name, size_t size) {
	return dl->slt_alloc(name, size);
}

void sltfree_addr(void *addr) {
	dl->slt_free_addr(addr);
}

void sltfree_name(const char *name) {
	dl->slt_free_name(name);
}

struct slt32_entry *sltget_addr(void *addr) {
	return dl->slt_get_addr(addr);
}

struct slt32_entry *sltget_name(const char *name) {
	return dl->slt_get_name(name);
}
