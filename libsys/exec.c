#include <kernel.h>
#include <config.h>
#include <string.h>
#include <elf.h>

extern void set_stack(uint32_t addr);
extern void exec_jump(uint32_t ip, uint32_t sp);

// Expects executable at ESPACE address
int exec() {

	// Check executable validity
	if (elf_check((void*) ESPACE)) return 1; // Yay

	/***** THE POINT OF NO RETURN! *****/

	// Jump to temporary stack
	mmap_call(0, 0x1000, 0x7);
	set_stack(0xF00);

	// Clear current process address space
	umap_call(0x1000, ESPACE - 0x1000);
	mmgc_call();

	// Load executable
	elf_load((void*) ESPACE);
	exec_jump(elf_entry((void*) ESPACE), USTACK_INI);

	return 0;
}
