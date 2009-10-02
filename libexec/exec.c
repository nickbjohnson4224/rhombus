#include <kernel.h>
#include <config.h>
#include <string.h>
#include <elf.h>

extern void set_stack(uint32_t addr);
extern void exec_jump(uint32_t ip, uint32_t sp);
int _exec(void);

/* Bootstraps and runs execution code */
int _load_exec(uint8_t *image, size_t size) {
	extern uint8_t _sxtext, _extext;
	uintptr_t szxtext;
	int (*_exec_location)(void);

	if (size > 0xC00000) return 1; /* Max 12MB static executables */

	/* Find size of exec code */
	szxtext = (uintptr_t) &_extext - (uintptr_t) &_sxtext;

	/* Copy exec code to EXEC_STRAP */
	mmap_call(EXEC_STRAP, szxtext, MMAP_RW | MMAP_EXEC);
	memcpy((void*) EXEC_STRAP, &_sxtext, szxtext);

	/* Copy executable to ESPACE */
	mmap_call(ESPACE, size, MMAP_RW);
	memcpy((void*) ESPACE, image, size);

	_exec_location = (int (*)(void)) ((uintptr_t) _exec - (uintptr_t) &_sxtext + EXEC_STRAP);
	return _exec_location();

	return 0;
}

/* Expects executable at ESPACE address */
__attribute__ ((section(".xtext"))) 
int _exec() {

	/* Check executable validity */
	if (elf_check((void*) ESPACE)) return 1;

	/***** THE POINT OF NO RETURN! *****/

	/* Jump to temporary stack */
	mmap_call(0, 0x1000, 0x7);
	set_stack(0xF00);

	/* Clear current process address space */
	umap_call(0x1000, ESPACE - 0x1000);

	/* Load executable */
	elf_load((void*) ESPACE);
	exec_jump(elf_entry((void*) ESPACE), USTACK_INI);

	return 0;
}
