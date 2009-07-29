// Copyright 2009 Nick Johnson

#include <lib.h>
#include <exec.h>

static int elf_null_segment(elf_t *header, elf_ph_t *segment);
static int elf_load_segment(elf_t *header, elf_ph_t *segment);
static int elf_note_segment(elf_t *header, elf_ph_t *segment);
static int elf_phdr_segment(elf_t *header, elf_ph_t *segment);

typedef int (*elf_segment_handler_t) (elf_t *header, elf_ph_t *segment);

static elf_segment_handler_t elf_segment_handler[0x100] = {
elf_null_segment,
elf_load_segment,
NULL, NULL,
elf_note_segment,
NULL,
elf_phdr_segment
};

int elf_segment_load(elf_t *header, elf_ph_t *segment) {
	if (segment->p_type > 7) return ESUPPORT;
	if (elf_segment_handler[segment->p_type])
		return elf_segment_handler[segment->p_type](header, segment);
	else
		return ESUPPORT;
}

static int elf_null_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}

static int elf_load_segment(elf_t *header, elf_ph_t *segment) {	
	u8int *mem_base = (void*) segment->p_vaddr;
	u32int file_off = segment->p_offset;
	u8int *file_base = (u8int*) header;
	char buffer[10];

	// Allocate memory
	eout("ELF loading segment ");
	eout(itoa(mem_base, buffer, 16)); 
	eout(" size ");
	eout(itoa(segment->p_memsz, buffer, 16));
	eout("\n");
	mmap(mem_base, segment->p_memsz, 0x7);

	// Copy data
	memcpy(mem_base, &file_base[file_off], segment->p_filesz);

	eout("ELF segment end\n");

	return 0;
}

static int elf_note_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}

static int elf_phdr_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}
