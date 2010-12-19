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

#include <stdlib.h>
#include <string.h>
#include <arch.h>
#include <page.h>

/* ELF header ***************************************************************/

#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define EI_CLASS	4
#define EI_DATA		5
#define EI_VERSION	6
#define EI_PAD		7
#define EI_NIDENT	16

struct elf32_ehdr {
	uint8_t  e_ident[EI_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC 	2
#define ET_DYN		3

#define EM_386  	3

#define ELFMAG0		0x7F
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

int  elf_check_file(struct elf32_ehdr *file);
void elf_load_file (struct elf32_ehdr *file);

/* ELF program header *******************************************************/

struct elf32_phdr {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};

#define PT_NULL 	0
#define PT_LOAD 	1
#define PT_DYNAMIC 	2
#define PT_INTERP 	3
#define PT_NOTE 	4
#define PT_SHLIB 	5
#define PT_PHDR		6

#define PF_R	0x1
#define PF_W	0x2
#define PF_X	0x4

uint32_t dl_entry;

/*****************************************************************************
 * dl_load
 *
 * Load the dynamic linker <dl_image>. This is usually done only at init, but
 * can be done later if a different linker is needed.
 */

int dl_load(void *dl_image) {
	struct elf32_ehdr *dl_elf;
	struct elf32_phdr *phdr;
	size_t phdr_count;
	size_t i;
	int prot;
	uintptr_t dst, src;
	
	dl_elf = dl_image;

	/* check ELF header */
	if (dl_elf->e_ident[EI_MAG0] != ELFMAG0) return 1;
	if (dl_elf->e_ident[EI_MAG1] != ELFMAG1) return 1;
	if (dl_elf->e_ident[EI_MAG2] != ELFMAG2) return 1;
	if (dl_elf->e_ident[EI_MAG3] != ELFMAG3) return 1;
	if (dl_elf->e_type           != ET_DYN)  return 1;
	if (dl_elf->e_machine        != EM_386)  return 1;
	if (dl_elf->e_version        != 1)       return 1;

	/* get program header table */
	phdr = (void*) ((uintptr_t) dl_image + dl_elf->e_phoff);
	phdr_count = dl_elf->e_phnum;

	/* load all loadable segments */
	for (i = 0; i < phdr_count; i++) {
		if (phdr[i].p_type == PT_LOAD) {

			dst = phdr[i].p_vaddr + DL_BASE;
			src = phdr[i].p_offset + (uintptr_t) dl_image;
			
			/* calculate page flags */
			prot = 0;
			if (phdr[i].p_flags & PF_R) prot |= PROT_READ;
			if (phdr[i].p_flags & PF_W) prot |= PROT_WRITE;
			if (phdr[i].p_flags & PF_X) prot |= PROT_EXEC;

			/* allocate memory */
			page_anon((void*) dst, phdr[i].p_filesz, prot | PROT_WRITE);

			/* copy segment */
			memcpy((void*) dst, (void*) src, phdr[i].p_filesz);
			memclr((void*) (dst + phdr[i].p_filesz), phdr[i].p_memsz - phdr[i].p_filesz);

			/* fix flags */
			page_prot((void*) dst, phdr[i].p_filesz, prot);
		}
	}

	/* get and relocate entry */
	dl_entry = dl_elf->e_entry + DL_BASE;

	return 0;
}
