/* Flux dynamic linker linker script */
ENTRY(_dl_entry)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {

	. = 0xC0000000;

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
