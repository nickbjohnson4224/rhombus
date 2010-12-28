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

#include "dl.h"

static void _elf_load_phdr(struct elf32_ehdr *file, struct elf32_phdr *phdr) {
	uint8_t *file_base;
	uint8_t *seg_base;
	uint8_t *dst;
	int prot;
	
	/* get pointer to start of file */
	file_base = (void*) file;

	switch (phdr->p_type) {
	case PT_LOAD:
		/* load segment */
	
		/* get pointer to start of segment */
		seg_base = &file_base[phdr->p_offset];
		
		/* get pointer to destination */
		dst = (void*) phdr->p_vaddr;

		/* allocate memory */
		dl_page_anon(dst, phdr->p_memsz, PROT_READ | PROT_WRITE);

		/* copy data */
		dl_memcpy(dst, seg_base, phdr->p_filesz);

		/* clear remaining space */
		dl_memclr(&dst[phdr->p_filesz], phdr->p_memsz - phdr->p_filesz);

		/* set proper permissions */
		prot = 0;
		if (phdr->p_flags & PF_R) prot |= PROT_READ;
		if (phdr->p_flags & PF_W) prot |= PROT_WRITE;
		if (phdr->p_flags & PF_X) prot |= PROT_EXEC;

		dl_page_prot(dst, phdr->p_memsz, prot);
		
		break;
	default:
		break;
	}
}

int dl_elf_load(struct elf32_ehdr *file) {
	struct elf32_phdr *phdr_tbl;
	size_t i;

	phdr_tbl = (void*) ((uintptr_t) file + file->e_phoff);

	for (i = 0; i < file->e_phnum; i++) {
		_elf_load_phdr(file, &phdr_tbl[i]);
	}

	return 0;
}

int dl_elf_check(struct elf32_ehdr *file) {

	if (file->e_ident[EI_MAG0] != ELFMAG0) return 1;
	if (file->e_ident[EI_MAG1] != ELFMAG1) return 1;
	if (file->e_ident[EI_MAG2] != ELFMAG2) return 1;
	if (file->e_ident[EI_MAG3] != ELFMAG3) return 1;
	if (file->e_machine        != EM_386)  return 1;
	if (file->e_version        != 1)       return 1;

	return 0;
}
