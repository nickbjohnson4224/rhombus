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

			/* move memory */
			page_self((void*) seg_base, dst, phdr->p_filesz);
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

	phdr_tbl = elf_get_segtab(image);

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

const struct elf32_phdr *elf_get_segtab(const struct elf32_ehdr *image) {
	return (const struct elf32_phdr*) ((uintptr_t) image + image->e_phoff);
}

uintptr_t elf_get_vsize(const struct elf32_ehdr *image) {
	const struct elf32_phdr *segtab;
	size_t i;

	segtab = elf_get_segtab(image);

	for (i = 0; segtab[i].p_type == PT_LOAD || segtab[i].p_type == PT_DYNAMIC; i++);
	i--;

	return segtab[i].p_vaddr + segtab[i].p_memsz;
}

const struct elf32_dyn *elf_get_dynamic(const struct elf32_ehdr *image) {
	const struct elf32_phdr *segtab;
	size_t i;

	segtab = elf_get_segtab(image);

	for (i = 0; segtab[i].p_type != PT_NULL; i++) {
		if (segtab[i].p_type == PT_DYNAMIC) {
			return (const struct elf32_dyn*) ((uintptr_t) image + segtab[i].p_offset);
		}
	}

	return NULL;
}

const struct elf32_sym *elf_get_symtab(const struct elf32_ehdr *image) {
	const struct elf32_dyn *dynamic;
	size_t i;

	dynamic = elf_get_dynamic(image);

	if (!dynamic) {
		/* no DYNAMIC segment */
		return NULL;
	}

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_SYMTAB) {
			return (const struct elf32_sym*) ((uintptr_t) image + dynamic[i].val);
		}
	}

	/* no symbol table in DYNAMIC segment */
	return NULL;
}

const char *elf_get_strtab(const struct elf32_ehdr *image) {
	const struct elf32_dyn *dynamic;
	size_t i;

	dynamic = elf_get_dynamic(image);

	if (!dynamic) {
		/* no DYNAMIC segment */
		return NULL;
	}

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_STRTAB) {
			return (const char*) ((uintptr_t) image + dynamic[i].val);
		}
	}

	/* no string table in DYNAMIC segment */
	return NULL;
}

const struct elf32_rel *elf_get_reltab(const struct elf32_ehdr *image, size_t *count) {
	const struct elf32_dyn *dynamic;
	const struct elf32_rel *reltab = NULL;
	size_t i;

	dynamic = elf_get_dynamic(image);

	if (!dynamic) {
		/* no DYNAMIC segment */
		return NULL;
	}

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_REL) {
			reltab = (const struct elf32_rel*) ((uintptr_t) image + dynamic[i].val);
		}
		if (dynamic[i].tag == DT_RELSZ) {
			*count = dynamic[i].val / 8;
		}
	}

	/* no REL in DYNAMIC segment */
	return reltab;
}

void elf_relocate_all(struct elf32_ehdr *image) {
	const struct elf32_rel *reltab = NULL;
	const struct elf32_sym *symtab = NULL;
	uint32_t *image32 = (void*) image;
	size_t count = 0;
	size_t i;

	reltab = elf_get_reltab(image, &count);
	symtab = elf_get_symtab(image);

	if (!reltab || !symtab) {
		return;
	}
	
	for (i = 0; i < count; i++) {
		switch (ELF32_R_TYPE(reltab[i].r_info)) {
		case R_386_NONE:
			break;
		case R_386_RELATIVE:
			image32[reltab[i].r_offset / 4] += (uint32_t) image;
			break;
		case R_386_GLOB_DAT:
		case R_386_JMP_SLOT:
			image32[reltab[i].r_offset / 4] 
				= symtab[ELF32_R_SYM(reltab[i].r_info)].st_value 
					+ (uint32_t) image;
			break;
		case R_386_32:
			image32[reltab[i].r_offset / 4] 
				+= symtab[ELF32_R_SYM(reltab[i].r_info)].st_value 
					+ (uint32_t) image;
			break;
		default:
			break;
		}
	}
}

const char *elf_get_soname(const struct elf32_ehdr *image) {
	const struct elf32_dyn *dynamic;
	const char *strtab;
	size_t i;

	strtab = elf_get_strtab(image);

	if (!strtab) {
		return NULL;
	}

	dynamic = elf_get_dynamic(image);

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_SONAME) {
			return &strtab[dynamic[i].val];
		}
	}

	/* no soname table in DYNAMIC segment */
	return NULL;
}

const struct elf32_sym *elf_get_symbol(const struct elf32_ehdr *image, const char *symbol) {
	const struct elf32_sym *symtab;
	const char *strtab;
	size_t i;

	strtab = elf_get_strtab(image);
	symtab = elf_get_symtab(image);

	if (!strtab || !symtab) {
		return NULL;
	}

	// FIXME - determine length of table
	for (i = 1;; i++) {
		if (!strcmp(&strtab[symtab[i].st_name], symbol)) {
			return &symtab[i];
		}
	}

	return NULL;
}
