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
#include <string.h>
#include <space.h>
#include <elf.h>

/*****************************************************************************
 * elf_load_phdr
 *
 * Attempt to load an ELF segment from the ELF file <file> and the program 
 * header <phdr>. If the segment is not of the type PT_LOAD or PT_DYNAMIC, 
 * nothing is loaded.
 */

void elf_load_phdr(struct elf32_ehdr *file, struct elf32_phdr *phdr) {
	uint8_t *file_base;
	uint8_t *seg_base;
	uint8_t *dst;
	frame_t pflags;
	
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
		pflags = PF_USER | PF_PRES;

		if (phdr->p_flags & PF_W) {
			pflags |= PF_RW;
		}

		mem_alloc((uintptr_t) dst, phdr->p_memsz, pflags);

		/* copy data */
		memcpy(dst, seg_base, phdr->p_filesz);

		/* clear remaining space */
		memclr(dst + phdr->p_filesz, phdr->p_memsz - phdr->p_filesz);
		
		break;
	default:
		break;
	}
}
