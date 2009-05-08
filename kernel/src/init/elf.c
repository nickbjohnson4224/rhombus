// Copyright 2009 Nick Johnson

#include <lib.h>
#include <elf.h>
#include <task.h>
#include <mem.h>

__attribute__ ((section(".ttext")))
void elf_load_segment(u8int *src, elf_ph_t *seg) {

	// Check if we can load this segment
	if (seg->p_type != PT_LOAD) return; // No libraries or anything else!

	// Get pointer to source
	u8int *src_base = &src[seg->p_offset];
	u32int src_limit = (u32int) src_base + seg->p_filesz;

	// Get pointer to destination
	u8int *dest_base = (u8int*) seg->p_vaddr;
	u32int dest_limit = ((u32int) dest_base + seg->p_memsz + 0x1000) & ~0xFFF;

	// Allocate adequate memory
	task_t *t = get_task(curr_pid);
	map_load(&t->map);
	u32int i = ((u32int) dest_base) & ~0xFFF;
	for (; i < dest_limit; i += 0x1000) {
		p_alloc(&t->map, i, PF_USER | PF_RW | PF_PRES);
	}

	// Copy data
	memcpy(dest_base, src_base, seg->p_memsz);

	// Set proper flags (i.e. remove write flag if needed)
	if (seg->p_flags & PF_W) {
		i = ((u32int) dest_base) & ~0xFFF;
		for (; i < dest_limit; i+= 0x1000) {
			page_set(&t->map, i, page_fmt(page_get(&t->map, i), PF_USER | PF_PRES));
		}
	}

}

__attribute__ ((section(".ttext")))
int elf_check(u8int *src) {
	elf_t *elf_header = (elf_t*) src;
	if (elf_header->e_ident[0] != 0x7F)return 1;
	if (elf_header->e_ident[1] != 'E') return 1;
	if (elf_header->e_ident[2] != 'L') return 1;
	if (elf_header->e_ident[3] != 'F') return 1;
	if (elf_header->e_type != ET_EXEC) return 1;
	if (elf_header->e_machine != EM_386) return 1;
	if (elf_header->e_version == 0) return 1;
	return 0;
}

__attribute__ ((section(".ttext")))
u32int elf_load(u8int *src) {
	elf_t *elf_header = (elf_t*) src;

	// Load all segments
	elf_ph_t *program_header = (elf_ph_t*) &src[elf_header->e_phoff];
	u32int n = elf_header->e_phnum;
	if (!n) return (u32int) NULL;
	u32int i;
	for (i = 0; i < n; i++) elf_load_segment(src, &program_header[i]);
	
	return elf_header->e_entry;
}
