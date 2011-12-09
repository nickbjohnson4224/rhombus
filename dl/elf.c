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

#include <string.h>
#include <rho/page.h>
#include <rho/layout.h>

#include "dl.h"

static void _elf_load_phdr(const struct elf32_ehdr *file, uintptr_t base, const struct elf32_phdr *phdr) {
	const uint8_t *file_base;
	const uint8_t *seg_base;
	uint8_t *dst;
	int prot;
	
	/* get pointer to start of file */
	file_base = (const void*) file;

	switch (phdr->p_type) {
	case PT_DYNAMIC:
	case PT_LOAD:
		/* load segment */
	
		/* get pointer to start of segment */
		seg_base = &file_base[phdr->p_offset];
		
		/* get pointer to destination */
		dst = (void*) (phdr->p_vaddr + base);

		if (phdr->p_flags & PF_W) {
			/* is a writable data segment */
		
			/* allocate memory */
			page_anon(dst, phdr->p_memsz, PROT_READ | PROT_WRITE);

			/* copy data */
			memcpy(dst, seg_base, phdr->p_filesz);

			/* clear rest of segment */
			memclr(&dst[phdr->p_filesz], phdr->p_memsz - phdr->p_filesz);
		}
		else {
			/* is a read-only data/code segment */

			if ((uint32_t) seg_base % PAGESZ) {
				/* image not page-aligned; copy */

				/* allocate memory */
				page_anon(dst, phdr->p_memsz, PROT_READ | PROT_WRITE);
				
				/* copy data */
				memcpy(dst, seg_base, phdr->p_filesz);

				/* clear rest of segment */
				memclr(&dst[phdr->p_filesz], phdr->p_memsz - phdr->p_filesz);
			}
			else {
				/* image page-aligned; page flip */

				/* move memory */
				page_self((void*) seg_base, dst, phdr->p_filesz);
			}
		}

		/* set proper permissions */
		prot = 0;
		if (phdr->p_flags & PF_R) prot |= PROT_READ;
		if (phdr->p_flags & PF_W) prot |= PROT_WRITE;
		if (phdr->p_flags & PF_X) prot |= PROT_EXEC;
		page_prot(dst, phdr->p_memsz, prot);
		
		break;
	default:
		break;
	}
}

int elf_load(const struct elf32_ehdr *image, uintptr_t base) {
	const struct elf32_phdr *phdr_tbl;
	size_t i;

	phdr_tbl = (const void*) ((uintptr_t) image + image->e_phoff);

	for (i = 0; i < image->e_phnum; i++) {
		_elf_load_phdr(image, base, &phdr_tbl[i]);
	}

	return 0;
}

int elf_check(const struct elf32_ehdr *file) {

	if (file->e_ident[EI_MAG0] != ELFMAG0) return 1;
	if (file->e_ident[EI_MAG1] != ELFMAG1) return 1;
	if (file->e_ident[EI_MAG2] != ELFMAG2) return 1;
	if (file->e_ident[EI_MAG3] != ELFMAG3) return 1;
	if (file->e_machine        != EM_386)  return 1;
	if (file->e_version        != 1)       return 1;

	return 0;
}
