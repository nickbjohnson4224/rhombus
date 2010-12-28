/* Flux dynamic linker linker script */
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xC0000000;

	/* header section */
	.head : {
		*(.head)
	}

	/* read-only sections (code) */
	.text : {
		*(.text)
		*(.rodata.*)
	}

	/* read-write sections (data) */
	. = ALIGN(0x1000);

	.data : { 
		*(.data)
	}

	.bss : {
		*(.bss)
	}
}
