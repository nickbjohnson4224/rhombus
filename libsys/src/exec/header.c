// Copyright 2009 Nick Johnson

#include <lib.h>
#include <exec.h>

int elf_load(elf_t *header, load_image_t *image) {
	int i;
	elf_ph_t *seg_base;

	if (!header || elf_header_check(header)) return 1;
	if (!image) return 2;

	seg_base = (void*) ((addr_t) header + (addr_t) header->e_phoff);

	for (i = 0; i < header->e_phnum; i++)
		elf_segment_load(header, &seg_base[i]);

	image->entry = header->e_entry;
	return 0;
}

int elf_header_check(elf_t *header) {
	if (header->e_ident[0] != 0x7F)return 1;
	if (header->e_ident[1] != 'E') return 1;
	if (header->e_ident[2] != 'L') return 1;
	if (header->e_ident[3] != 'F') return 1;
	if (header->e_type != ET_EXEC) return 1;
	if (header->e_machine != EM_SEL) return 1;
	if (header->e_version == 0) return 1;
	return 0;
}
