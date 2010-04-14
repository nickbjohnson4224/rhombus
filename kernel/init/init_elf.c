/* 
 * Copyright 2009, 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details 
 */

#include <util.h>
#include <elf.h>
#include <time.h>
#include <space.h>

void elf_load_segment(uint8_t *src, elf_ph_t *seg) {
	uint8_t *src_base, *dest_base;
	uint32_t dest_limit, i;

	/* Check if we can load this segment */
	if (seg->p_type != PT_LOAD) return; /* No libraries or any other crap! */

	/* Get pointer to source */
	src_base = &src[seg->p_offset];

	/* Get pointer to destination */
	dest_base = (uint8_t*) seg->p_vaddr;
	dest_limit = ((uint32_t) dest_base + seg->p_memsz + 0x1000) &~ 0xFFF;

	/* Allocate adequate memory */
	i = ((uint32_t) dest_base) &~ 0xFFF;
	for (; i < dest_limit; i += 0x1000) {
		page_set(i, page_fmt(frame_new(), PF_USER | PF_RW | PF_PRES));
	}

	/* Copy data */
	memcpy(dest_base, src_base, seg->p_filesz);

	/* Clear other space */
	memclr(dest_base + seg->p_filesz, seg->p_memsz - seg->p_filesz);
}

int elf_check(uint8_t *src) {
	elf_t *elf_header;
	
	elf_header = (elf_t*) src;
	
	if (elf_header->e_ident[0] != 0x7F)  return 1;
	if (elf_header->e_ident[1] != 'E')   return 1;
	if (elf_header->e_ident[2] != 'L')   return 1;
	if (elf_header->e_ident[3] != 'F')   return 1;
	if (elf_header->e_type != ET_EXEC)   return 1;
	if (elf_header->e_machine != EM_386) return 1;
	if (elf_header->e_version == 0)      return 1;

	return 0;
}

uint32_t elf_load(uint8_t *src) {
	uint32_t i, n;
	elf_t *elf_header = (elf_t*) src;

	/* Load all segments */
	elf_ph_t *program_header = (elf_ph_t*) &src[elf_header->e_phoff];
	n = elf_header->e_phnum; /* Number of segments */
	if (!n) return (uint32_t) NULL;

	for (i = 0; i < n; i++) elf_load_segment(src, &program_header[i]);
	
	return elf_header->e_entry; /* Return entry point */
}
