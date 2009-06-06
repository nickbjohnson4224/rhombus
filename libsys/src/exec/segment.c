// Copyright 2009 Nick Johnson

#include <lib.h>
#include <exec.h>

static int elf_null_segment(elf_t *header, elf_ph_t *segment);
static int elf_load_segment(elf_t *header, elf_ph_t *segment);
static int elf_dynm_segment(elf_t *header, elf_ph_t *segment);
static int elf_intr_segment(elf_t *header, elf_ph_t *segment);
static int elf_note_segment(elf_t *header, elf_ph_t *segment);
static int elf_shar_segment(elf_t *header, elf_ph_t *segment);
static int elf_phdr_segment(elf_t *header, elf_ph_t *segment);

typedef int (*elf_segment_handler_t) (elf_t *header, elf_ph_t *segment);

static elf_segment_handler_t elf_segment_handler[] = {
elf_null_segment,
elf_load_segment,
elf_dynm_segment,
elf_intr_segment,
elf_note_segment,
elf_shar_segment,
elf_phdr_segment
};

int elf_segment_load(elf_t *header, elf_ph_t *segment) {
	char buffer[10];
	eout("Loading segment to 0x");
	eout(itoa(segment->p_vaddr, buffer, 16));
	eout(", type ");
	eout(itoa(segment->p_type, buffer, 10));
	eout("\n");
	return elf_segment_handler[segment->p_type](header, segment);
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
	for (i = (u32int) mem_base; i < (u32int) mem_base + segment->p_memsz; i += 0x1000) {
		eout("Allocating page 0x");
		eout(itoa(i, buffer, 16));
		eout(" result ");
		eout(itoa(mmap_call(i, (segment->p_flags & PF_W) ? 0x7 : 0x5), buffer, 10));
		eout("\n");
	}

	// Copy data
	memcpy(mem_base, &file_base[file_off], segment->p_filesz);

	return 0;
}

static int elf_dynm_segment(elf_t *header, elf_ph_t *segment) {
	return ESUPPORT;
}

static int elf_intr_segment(elf_t *header, elf_ph_t *segment) {
	return ESUPPORT;
}

static int elf_note_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}

static int elf_shar_segment(elf_t *header, elf_ph_t *segment) {
	return ESUPPORT;
}

static int elf_phdr_segment(elf_t *header, elf_ph_t *segment) {
	return ENULLSEG;
}
