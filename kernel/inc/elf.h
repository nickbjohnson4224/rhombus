#ifndef ELF_H
#define ELF_H

#include <lib.h>

/* ELF Header */
typedef struct {
	uint8_t e_ident[16];
	uint16_t e_type, e_machine;
	uint32_t e_version;
	uint32_t e_entry, e_phoff, e_shoff, e_flags;
	uint16_t e_ehsize, e_phentsize, e_phnum;
	uint16_t e_shentsize, e_shnum, e_shstrndx;
} __attribute__ ((packed)) elf_t;

#define ET_EXEC 2
#define EM_386 3

/* ELF Program Header */
typedef struct {
	uint32_t p_type, p_offset, p_vaddr, p_paddr;
	uint32_t p_filesz, p_memsz, p_flags, p_align;
} __attribute__ ((packed)) elf_ph_t;

#define PT_NULL 	0
#define PT_LOAD 	1
#define PT_DYNAMIC 	2
#define PT_INTERP 	3
#define PT_NOTE 	4
#define PT_SHLIB 	5
#define PT_PHDR		6

#define PF_R	0x1
#define PF_W	0x2
#define PF_X	0x4

void elf_load_segment(uint8_t *src, elf_ph_t *seg);
uint32_t elf_load(uint8_t *src); /* Returns entry point */
int elf_check(uint8_t *src);

#endif /*ELF_H*/
