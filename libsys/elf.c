#include <elf.h>
#include <kernel.h>
#include <string.h>

static int elf_load_segment(elf_ph_t *pheader, uint8_t *base);

typedef int (*elf_segment_handler_t)(elf_ph_t *, uint8_t *);
static int elf_segment_err(elf_ph_t *pheader, uint8_t *base);
static int elf_segment_null(elf_ph_t *pheader, uint8_t *base);
static int elf_segment_load(elf_ph_t *pheader, uint8_t *base);

int elf_check(elf_t *header) {

	if (header->e_ident[0] != 0x7F) return 1;
	if (header->e_ident[1] != 'E') return 1;
	if (header->e_ident[2] != 'L') return 1;
	if (header->e_ident[3] != 'F') return 1;
	if (header->e_type != ET_EXEC) return 1;
	if (header->e_machine != EM_386) return 1;
	if (header->e_version == 0) return 1;

	return 0;
}

int elf_load(elf_t *header) {
	uint8_t *base = (void*) header;
	elf_ph_t *pheader_table = (void*) &base[header->e_phoff];
	uint32_t pheader_table_size = header->e_phnum;
	uint32_t i;

	for (i = 0; i < pheader_table_size; i++) {
		if (elf_load_segment(&pheader_table[i], base)) {
			return 1;
		}
	}

	return 0;
}

uint32_t elf_entry(elf_t *header) {
	return header->e_entry;
}

static int elf_load_segment(elf_ph_t *pheader, uint8_t *base) {
	elf_segment_handler_t handler[] = {
		elf_segment_null,
		elf_segment_load,
		elf_segment_null,
		elf_segment_err,
		elf_segment_null,
		elf_segment_err,
		elf_segment_null
	};

	if (pheader->p_type > 7) return elf_segment_null(pheader, base);
	else return handler[pheader->p_type](pheader, base);
}

static int elf_segment_err(elf_ph_t *pheader, uint8_t *base) { return 1; }
static int elf_segment_null(elf_ph_t *pheader, uint8_t *base) { return 0; }

static int elf_segment_load(elf_ph_t *pheader, uint8_t *base) {
	uint8_t flags = 0;

	if (pheader->p_flags & PF_W) flags |= MMAP_RW;
	if (pheader->p_flags & PF_X) flags |= MMAP_EXEC;

	mmap_call(pheader->p_vaddr, pheader->p_memsz, flags);

	memcpy((void*) pheader->p_vaddr, (void*) &base[pheader->p_offset], pheader->p_filesz);
	memclr((void*) (pheader->p_vaddr + pheader->p_filesz), pheader->p_memsz - pheader->p_filesz);

	return 0;
}
