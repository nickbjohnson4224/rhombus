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

/*****************************************************************************
 * elfc_relocate
 *
 * Perform the relocation <rel> on the cached ELF image <cache>.
 */

uint32_t elfc_relocate(struct elf_cache *cache, const struct elf32_rel *rel) {
	const struct elf32_sym *symbol = NULL;
	uint32_t *image32 = (void*) cache->image;
	uint32_t symbol_value;

	/* compute symbol value if needed */
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_GLOB_DAT:
	case R_386_JMP_SLOT:
	case R_386_32:
		symbol = &cache->symtab[ELF32_R_SYM(rel->r_info)];
		
		if (symbol->st_value) {
			symbol_value = symbol->st_value + (uint32_t) cache->image;
		}
		else {
			symbol_value = elfc_resolve(cache, &cache->strtab[symbol->st_name]);
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
		image32[rel->r_offset / 4] += (uint32_t) cache->image;
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

uint32_t elfc_relocate_exec(struct elf_cache *cache, const struct elf32_rel *rel) {
	const struct elf32_sym *symbol = NULL;
	uint32_t *image32 = (void*) 0;
	uint32_t symbol_value;

	/* compute symbol value if needed */
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_GLOB_DAT:
	case R_386_JMP_SLOT:
	case R_386_32:
		symbol = &cache->symtab[ELF32_R_SYM(rel->r_info)];
		
		if (symbol->st_value) {
			symbol_value = symbol->st_value;
		}
		else {
			symbol_value = elfc_resolve(cache, &cache->strtab[symbol->st_name]);
		}
		break;
	default:
		symbol_value = 0;
	}

	/* perform relocation */
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_NONE:
	case R_386_RELATIVE:
		break;
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

/*****************************************************************************
 * elfc_relocate_all
 *
 * Perform all initial relocations on the cached ELF image <cache>.
 */

void elfc_relocate_all(struct elf_cache *cache) {
	size_t i;

	/* only relocate dynamic objects */
	if (!cache->dynamic) {
		return;
	}
	
	/* add special entries to GOT for PLT resolution */
	if (cache->pltgot) {
		((uint32_t*) cache->pltgot)[1] = (uint32_t) cache->image;
		((uint32_t*) cache->pltgot)[2] = (uint32_t) __plt_resolve;
	}

	/* perform normal relocations (data) */
	if (cache->reltab) {
		for (i = 0; i < cache->reltabn; i++) {
			elfc_relocate(cache, &cache->reltab[i]);
		}
	}

	/* do stub relocations for PLT GOT entries */
	if (cache->jmprel) {
		for (i = 0; i < cache->jmpreln; i++) {
			((uint32_t*) cache->image)[cache->jmprel[i].r_offset / 4] += (uint32_t) cache->image;
		}
	}
}

void elfc_relocate_all_exec(struct elf_cache *cache) {
	size_t i;

	/* only relocate dynamic objects */
	if (!cache->dynamic) {
		return;
	}

	/* add special entries to GOT for PLT resolution */
	if (cache->pltgot) {
		((uint32_t*) cache->pltgot)[1] = (uint32_t) cache->image;
		((uint32_t*) cache->pltgot)[2] = (uint32_t) __plt_resolve;
	}

	/* perform normal relocations (data) */
	if (cache->reltab) {
		for (i = 0; i < cache->reltabn; i++) {
			elfc_relocate_exec(cache, &cache->reltab[i]);
		}
	}
}

/*****************************************************************************
 * elfc_get_needed
 *
 * Return a pointer to the <index>th shared object requested to be loaded by
 * the cached ELF image <cache>.
 */

static const struct elf32_ehdr *elfc_get_needed(struct elf_cache *cache, size_t index) {
	const struct slt32_entry *slt;
	const char *soname;
	char objname[28];
	size_t i;

	soname = NULL;
	for (i = 0; cache->dynamic[i].tag != DT_NULL; i++) {
		if (cache->dynamic[i].tag == DT_NEEDED) {
			if (!index--) {
				soname = &cache->strtab[cache->dynamic[i].val];
			}
		}
	}

	if (!soname) {
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

/*****************************************************************************
 * elfc_hash
 *
 * String hashing function defined in the ELF documentation.
 */

static uint32_t elfc_hash(const char *symbol) {
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

/*****************************************************************************
 * elfc_get_symbol
 *
 * Find the symbol table entry for the symbol <symbol> in the cached ELF 
 * image <cache>. Returns NULL on failure.
 */

static const struct elf32_sym *elfc_get_symbol(struct elf_cache *cache, const char *symbol) {
	const uint32_t *bucket;
	const uint32_t *chain;
	uint32_t nbucket;
	uint32_t nchain;
	uint32_t hash;
	uint32_t i;

	nbucket = cache->hash[0];
	nchain  = cache->hash[1];
	bucket = &cache->hash[2];
	chain  = &cache->hash[2 + nbucket];

	hash = elfc_hash(symbol) % nbucket;
	i = bucket[hash];
	while (1) {

		if (i == 0) {
			return NULL;
		}

		if (!strcmp(&cache->strtab[cache->symtab[i].st_name], symbol)) {
			/* found symbol */
			return &cache->symtab[i];
		}

		i = chain[i];
	}

	return NULL;
}

/*****************************************************************************
 * elf_resolve_local
 *
 * Find the active value of the symbol <symbol> in the cached ELF image
 * <cache>. Returns 0 on failure.
 */

static uint32_t elfc_resolve_local(struct elf_cache *cache, const char *symbol) {
	const struct elf32_sym *syment;

	syment = elfc_get_symbol(cache, symbol);
	if (!syment || !syment->st_value) {
		return 0;
	}

	return (syment->st_value + (uint32_t) cache->image);
}

/*****************************************************************************
 * elfc_resolve_rec
 *
 * Part of elfc_resolve. Recursively performs a depth-first search on the
 * dependency tree of <cache>, checking only leaf nodes for symbols. The 
 * breadth-first search is implemented as an iterated depth-first search.
 */

static uint32_t elfc_resolve_rec(struct elf_cache *cache, const char *symbol, uint32_t depth) {
	const struct elf32_ehdr *dep;
	struct elf_cache dep_cache;
	uint32_t value;
	size_t i;
	
	if (depth == 0) {
		return elfc_resolve_local(cache, symbol);
	}

	i = 0;
	while (1) {
		dep = elfc_get_needed(cache, i);
		if (!dep) return 0;
		
		elf_gencache(&dep_cache, dep);
		value = elfc_resolve_rec(&dep_cache, symbol, depth - 1);
		if (value) return value;

		i++;
	}

	return 0;
}

/*****************************************************************************
 * elfc_resolve
 *
 * Find the active value of the symbol <symbol> by performing a breadth-first 
 * search on the dependency tree of <cache>. If no matching symbol is found, 
 * 0 is returned.
 */

uint32_t elfc_resolve(struct elf_cache *cache, const char *symbol) {
	uint32_t value;
	uint32_t depth;
	
	for (depth = 0; depth < 10; depth++) {
		value = elfc_resolve_rec(cache, symbol, depth);
		
		if (value) {
			return value;
		}
	}

	return 0;
}

/*****************************************************************************
 * elf_gencache
 *
 * Generate an ELF cached image from an ELF image, storing the cache data in
 * <cache>. This cache data is used to make lookups of various information
 * about the image much faster.
 */

void elf_gencache(struct elf_cache *cache, const struct elf32_ehdr *image) {
	uint32_t base = (uint32_t) image;
	size_t i;

	cache->image = image;

	/* get segment table */
	cache->segtab = (const struct elf32_phdr*) (base + image->e_phoff);

	/* calulate image size */
	for (i = 0; cache->segtab[i].p_type == PT_LOAD || cache->segtab[i].p_type == PT_DYNAMIC; i++);
	if (i) cache->vsize = cache->segtab[i - 1].p_vaddr + cache->segtab[i - 1].p_memsz + 0x1000;

	/* get DYNAMIC segment */
	cache->dynamic = NULL;
	for (i = 0; cache->segtab[i].p_type != PT_NULL; i++) {
		if (cache->segtab[i].p_type == PT_DYNAMIC) {
			cache->dynamic = (const struct elf32_dyn*) (base + cache->segtab[i].p_offset);
			break;
		}
	}

	if (!cache->dynamic) {
		return;
	}

	/* get various DYNAMIC values */
	cache->symtab = NULL;
	cache->reltab = NULL;
	cache->jmprel = NULL;
	cache->strtab = NULL;
	cache->pltgot = NULL;
	cache->hash   = NULL;
	cache->soname = NULL;
	for (i = 0; cache->dynamic[i].tag != DT_NULL; i++) {
		switch (cache->dynamic[i].tag) {
		case DT_SYMTAB:   cache->symtab  = (const struct elf32_sym*) (base + cache->dynamic[i].val); break;
		case DT_REL:      cache->reltab  = (const struct elf32_rel*) (base + cache->dynamic[i].val); break;
		case DT_RELSZ:    cache->reltabn = cache->dynamic[i].val / 8; break;
		case DT_JMPREL:   cache->jmprel  = (const struct elf32_rel*) (base + cache->dynamic[i].val); break;
		case DT_PLTRELSZ: cache->jmpreln = cache->dynamic[i].val / 8; break;
		case DT_STRTAB:   cache->strtab  = (const char*) (base + cache->dynamic[i].val); break;
		case DT_PLTGOT:   cache->pltgot  = (const uint32_t*) (base + cache->dynamic[i].val); break;
		case DT_HASH:     cache->hash    = (const uint32_t*) (base + cache->dynamic[i].val); break;
		case DT_SONAME:   cache->soname  = (const char*) cache->dynamic[i].val; break;
		}
	}

	cache->soname = (cache->soname) ? &cache->strtab[(uintptr_t) cache->soname] : NULL;
}

/*****************************************************************************
 * elf_gencache_exec
 *
 * Generate an ELF cached image from an ELF image, storing the cache data in
 * <cache>. This cache data is used to make lookups of various information
 * about the image much faster.
 */

void elf_gencache_exec(struct elf_cache *cache, const struct elf32_ehdr *image) {
	size_t i;

	cache->image = image;

	/* get segment table */
	cache->segtab = (const struct elf32_phdr*) (image->e_phoff + (uintptr_t) image);

	/* calulate image size */
	cache->vsize = 0;

	/* get DYNAMIC segment */
	cache->dynamic = NULL;
	for (i = 0; cache->segtab[i].p_type != PT_NULL; i++) {
		if (cache->segtab[i].p_type == PT_DYNAMIC) {
			cache->dynamic = (const struct elf32_dyn*) cache->segtab[i].p_vaddr;
			break;
		}
	}

	if (!cache->dynamic) {
		return;
	}

	/* get various DYNAMIC values */
	cache->symtab = NULL;
	cache->reltab = NULL;
	cache->jmprel = NULL;
	cache->strtab = NULL;
	cache->pltgot = NULL;
	cache->hash   = NULL;
	cache->soname = NULL;
	for (i = 0; cache->dynamic[i].tag != DT_NULL; i++) {
		switch (cache->dynamic[i].tag) {
		case DT_SYMTAB:   cache->symtab  = (const struct elf32_sym*) cache->dynamic[i].val; break;
		case DT_REL:      cache->reltab  = (const struct elf32_rel*) cache->dynamic[i].val; break;
		case DT_RELSZ:    cache->reltabn = cache->dynamic[i].val / 8; break;
		case DT_JMPREL:   cache->jmprel  = (const struct elf32_rel*) cache->dynamic[i].val; break;
		case DT_PLTRELSZ: cache->jmpreln = cache->dynamic[i].val / 8; break;
		case DT_STRTAB:   cache->strtab  = (const char*) cache->dynamic[i].val; break;
		case DT_PLTGOT:   cache->pltgot  = (const uint32_t*) cache->dynamic[i].val; break;
		case DT_HASH:     cache->hash    = (const uint32_t*) cache->dynamic[i].val; break;
		case DT_SONAME:   cache->soname  = (const char*) cache->dynamic[i].val; break;
		}
	}

	cache->soname = (cache->soname) ? &cache->strtab[(uintptr_t) cache->soname] : NULL;
}
