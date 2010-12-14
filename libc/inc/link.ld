/* Flux default linker script */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xC000;

	.event : { *(.event) }

	. = 0x10000;

	_stext = .;
	.text  : { *(.text) }
	_etext = .;

	_sdata = .;
	.data  : { *(.data) }
	_edata = .;

	_sbss = .;
	.bss   : { *(.bss)  }
	_ebss = .;
}
