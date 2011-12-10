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
#include <rho/exec.h>

#include "dl.h"

void *_load(void *image, size_t size, int flags) {
	struct elf32_ehdr *elf32 = image;
	struct elf_cache cache;
	char regname[28];
	void *object;

	/* check executable */
	if (!elf32 || elf_check(elf32)) {
		return NULL;
	}

	elf_gencache(&cache, elf32, 0);
	strlcpy(regname, "dl.obj:", 28);
	strlcat(regname, cache.soname, 28);

	object = sltalloc(regname, cache.vsize);
	elf_load(elf32, (uintptr_t) object);

	elf_gencache(&cache, object, 1);
	elfc_relocate_all(&cache);

	return (void*) object;
}
