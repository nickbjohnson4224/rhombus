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

uint32_t elf_get_dynval(const struct elf32_dyn *dynamic, int32_t type, uint32_t index) {
	size_t i;

	if (!dynamic) {
		/* no DYNAMIC segment */
		return 0;
	}

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == type) {
			if (!index--) {
				return dynamic[i].val;
			}
		}
	}
	
	/* not found */
	return 0;
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

const uint32_t *elf_get_gotplt(const struct elf32_ehdr *image) {
	const struct elf32_dyn *dynamic;
	size_t i;

	dynamic = elf_get_dynamic(image);

	if (!dynamic) {
		/* no DYNAMIC segment */
		return NULL;
	}

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_PLTGOT) {
			return (const uint32_t*) ((uintptr_t) image + dynamic[i].val);
		}
	}

	/* no GOT in DYNAMIC segment */
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

const struct elf32_rel *elf_get_pltrel(const struct elf32_ehdr *image, size_t *count) {
	const struct elf32_dyn *dynamic;
	const struct elf32_rel *reltab = NULL;
	size_t i;

	dynamic = elf_get_dynamic(image);

	if (!dynamic) {
		/* no DYNAMIC segment */
		return NULL;
	}

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_JMPREL) {
			reltab = (const struct elf32_rel*) ((uintptr_t) image + dynamic[i].val);
		}
		if (dynamic[i].tag == DT_PLTRELSZ) {
			*count = dynamic[i].val / 8;
		}
	}

	/* no JMPREL in DYNAMIC segment */
	return reltab;
}

uint32_t elf_relocate(struct elf32_ehdr *image, const struct elf32_rel *rel, 
		const char *strtab, const struct elf32_sym *symtab) {

	const struct elf32_sym *symbol = NULL;
	uint32_t *image32 = (void*) image;
	uint32_t symbol_value;

	/* compute symbol value if needed */
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_GLOB_DAT:
	case R_386_JMP_SLOT:
	case R_386_32:
		symbol = &symtab[ELF32_R_SYM(rel->r_info)];
		
		if (symbol->st_value) {
			symbol_value = symbol->st_value + (uint32_t) image;
		}
		else {
			symbol_value = elf_resolve(image, &strtab[symbol->st_name]);
		}
		break;
	default:
		symbol_value = 0;
	}

	/* perform relocation */
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_NONE:
		break;
	case R_386_RELATIVE:
		image32[rel->r_offset / 4] += (uint32_t) image;
		break;
	case R_386_GLOB_DAT:
	case R_386_JMP_SLOT:
		image32[rel->r_offset / 4] = symbol_value;
		break;
	case R_386_32:
		image32[rel->r_offset / 4] += symbol_value;
		break;
	default:
		break;
	}

	return symbol_value;
}	

void elf_relocate_all(struct elf32_ehdr *image) {
	const struct elf32_rel *reltab = NULL;
	const struct elf32_sym *symtab = NULL;
	uint32_t *got = NULL;
	uint32_t *image32;
	const char *strtab;
	size_t count = 0;
	size_t i;

	symtab = elf_get_symtab(image);
	strtab = elf_get_strtab(image);
	got = (uint32_t*) elf_get_gotplt(image);
	
	got[1] = (uint32_t) image;
	got[2] = (uint32_t) __plt_resolve;

	reltab = elf_get_reltab(image, &count);
	if (reltab) {
		for (i = 0; i < count; i++) {
			elf_relocate(image, &reltab[i], strtab, symtab);
		}
	}

	reltab = elf_get_pltrel(image, &count);
	if (reltab) {
		image32 = (void*) image;
		for (i = 0; i < count; i++) {
			image32[reltab[i].r_offset / 4] += (uint32_t) image;
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

const uint32_t *elf_get_hash(const struct elf32_ehdr *image) {
	const struct elf32_dyn *dynamic;
	const char *strtab;
	size_t i;

	strtab = elf_get_strtab(image);

	if (!strtab) {
		return NULL;
	}

	dynamic = elf_get_dynamic(image);

	for (i = 0; dynamic[i].tag != DT_NULL; i++) {
		if (dynamic[i].tag == DT_HASH) {
			return (const uint32_t*) ((uintptr_t) image + dynamic[i].val);
		}
	}

	/* no hash table in DYNAMIC segment */
	return NULL;
}

const struct elf32_ehdr *elf_get_needed(const struct elf32_ehdr *image, size_t i) {
	const struct elf32_dyn *dynamic;
	const struct slt32_entry *slt;
	const char *strtab;
	const char *soname;
	char objname[28];

	dynamic = elf_get_dynamic(image);
	strtab  = elf_get_strtab(image);

	soname = &strtab[elf_get_dynval(dynamic, DT_NEEDED, i)];

	if (soname == strtab) {
		return NULL;
	}

	strlcpy(objname, "dl.so:", 28);
	strlcat(objname, soname, 28);

	slt = sltget_name(objname);

	if (!slt) {
		return NULL;
	}

	return (const void*) slt->base;
}

uint32_t elf_hash(const char *symbol) {
	uint32_t h;
	uint32_t g;

	h = 0;
	while (*symbol) {
		h = (h << 4) + *symbol;

		g = h & 0xF0000000;
		if (g) h ^= g >> 24;
		h &= ~g;

		symbol++;
	}

	return h;
}

const struct elf32_sym *elf_get_symbol(const struct elf32_ehdr *image, const char *symbol) {
	const struct elf32_sym *symtab;
	const uint32_t *hashtab;
	const uint32_t *bucket;
	const uint32_t *chain;
	const char *strtab;
	uint32_t nbucket;
	uint32_t nchain;
	uint32_t hash;
	uint32_t i;

	strtab  = elf_get_strtab(image);
	symtab  = elf_get_symtab(image);
	hashtab = elf_get_hash  (image);

	if (!strtab || !symtab || !hashtab) {
		return NULL;
	}

	nbucket = hashtab[0];
	nchain  = hashtab[1];
	bucket = &hashtab[2];
	chain  = &hashtab[2 + nbucket];

	hash = elf_hash(symbol) % nbucket;
	i = bucket[hash];
	while (1) {

		if (i == 0) {
			return NULL;
		}

		if (!strcmp(&strtab[symtab[i].st_name], symbol)) {
			return &symtab[i];
		}

		i = chain[i];
	}

	return NULL;
}

uint32_t elf_resolve_local(const struct elf32_ehdr *image, const char *symbol) {
	const struct elf32_sym *syment;

	syment = elf_get_symbol(image, symbol);
	if (!syment || !syment->st_value) {
		return 0;
	}

	return (syment->st_value + (uint32_t) image);
}

uint32_t elf_resolve_rec(const struct elf32_ehdr *image, const char *symbol, uint32_t depth) {
	const struct elf32_ehdr *dep;
	uint32_t value;
	size_t i;
	
	if (depth == 0) {
		return elf_resolve_local(image, symbol);
	}

	i = 0;
	while (1) {
		dep = elf_get_needed(image, i);
		if (!dep) {
			return 0;
		}
		
		value = elf_resolve_rec(dep, symbol, depth - 1);
		if (value) {
			return value;
		}

		i++;
	}

	return 0;
}

uint32_t elf_resolve(const struct elf32_ehdr *image, const char *symbol) {
	uint32_t value;
	uint32_t depth;
	
	for (depth = 0; depth < 10; depth++) {
		value = elf_resolve_rec(image, symbol, depth);
		
		if (value) {
			return value;
		}
	}

	return 0;
}
