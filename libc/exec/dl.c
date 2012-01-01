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
#include <stdlib.h>
#include <stdio.h>

#include <rho/layout.h>
#include <rho/page.h>
#include <rho/exec.h>
#include <rho/elf.h>

struct dl *dl = (void*) 0xC0000000;

/*****************************************************************************
 * load_dl
 *
 * Load the dynamic linker <dl_image>. This is usually done only at init, but
 * can be done later if a different linker is needed. The given object must
 * not have relocations, must be linked at 0xC0000000, and must have its entry
 * point at 0xC0000000.
 */

int load_dl(void *dl_image) {
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
	if (dl_elf->e_machine        != EM_386)  return 1;
	if (dl_elf->e_version        != 1)       return 1;

	/* get program header table */
	phdr = (void*) ((uintptr_t) dl_image + dl_elf->e_phoff);
	phdr_count = dl_elf->e_phnum;

	/* load all loadable segments */
	for (i = 0; i < phdr_count; i++) {
		if (phdr[i].p_type == PT_LOAD) {

			dst = phdr[i].p_vaddr;
			src = phdr[i].p_offset + (uintptr_t) dl_image;
			
			/* calculate page flags */
			prot = 0;
			if (phdr[i].p_flags & PF_R) prot |= PROT_READ;
			if (phdr[i].p_flags & PF_W) prot |= PROT_WRITE;
			if (phdr[i].p_flags & PF_X) prot |= PROT_EXEC;

			/* allocate memory */
			page_anon((void*) dst, phdr[i].p_memsz, prot | PROT_WRITE);

			/* copy segment */
			memcpy((void*) dst, (void*) src, phdr[i].p_filesz);
			memclr((void*) (dst + phdr[i].p_filesz), phdr[i].p_memsz - phdr[i].p_filesz);

			/* fix flags */
			page_prot((void*) dst, phdr[i].p_memsz, prot);
		}
	}

	return 0;
}

void *dlopen(const char *filename, int flags) {
	void *image;
	const char *depname;
	char *path;
	char *deppath;
	size_t i;

	if (filename) {
		path = ldpath_resolve(filename);
		image = load_exec(path);
		free(path);

		if (!image) {
			return NULL;
		}

		if ((flags & RTLD_LOCAL) == 0) {
			for (i = 0;; i++) {
				depname = dl->dep(image, i, 0);
				if (!depname) break;
	
				deppath = strvcat("/lib/", depname, NULL);
				dlopen(deppath, flags);
				free(deppath);
			}
		}

		return dl->load(image, msize(image), flags);
	}
	else {
		return (void*) (sltget_name("sys.exec")->base);
	}
}

void *dlsym(void *object, const char *symbol) {
	return dl->sym(object, symbol);
}

char *dlerror(void) {
	// XXX - stubbed
	return strdup("");
}

void dlclose(void *object) {
	dl->fini(object);
	dl->uload(object);
}
