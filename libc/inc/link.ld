/* Flux default linker script */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0x100000;

	/* read-only sections (code) */
	.text : { 
		_stext = .;
		*(.text)
		*(.rodata.*)
		_etext = .;
	}

	/* read-write sections (data) */
/*	. = ALIGN (0x1000); */

	.data : { 
		*(.data)
	}

	.bss : {
		*(.bss)
	}
}
