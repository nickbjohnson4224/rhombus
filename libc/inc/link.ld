/* Flux default linker script */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	/* read-only sections (code) */
	. = 0x10000;

	.text : { 
		_stext = .;
		*(.text)
		*(.rodata.*)
		_etext = .;
	}

	/* read-write sections (data) */
	. = ALIGN (0x1000);

	.data : { 
		_sdata = .;
		*(.data)
		_edata = .;
	}

	.bss : {
		_sbss = .;
		*(.bss)
		_ebss = .;
	}

	/* dynamic linker */
	. = ALIGN (0x1000);

	.dl : { 
		_dl_start = .;
		*(.dltext)
		*(.dldata)
		_dl_end = .;
	}
}
