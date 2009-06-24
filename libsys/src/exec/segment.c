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
	u32int i;
	char buffer[10];

	// Allocate memory
	for (i = (u32int) mem_base & ~0xFFF; i < (u32int) mem_base + segment->p_memsz; i += 0x1000)
		mmap_call(i, 0x7);

	// Copy data
	memcpy(mem_base, &file_base[file_off], segment->p_filesz);

	return 0;
}

static int elf_note_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}

static int elf_phdr_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}
