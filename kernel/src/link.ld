ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xFF100000;

	.text : AT(ADDR(.text) - 0xFF000000) {
		*(.text)
		*(.rodata*)
	}

	.data : AT(ADDR(.data) - 0xFF000000) {
		*(.data)
	}

	.tdata ALIGN (0x1000) : AT(ADDR(.tdata) - 0xFF000000) {
		START_OF_TEMP = .;
		*(.tdata)
	}

	.ttext : AT(ADDR(.ttext) - 0xFF000000) {
		*(.ttext)
		. = ALIGN(0x1000);
		END_OF_TEMP = .;
	}

	.bss : AT(ADDR(.bss) - 0xFF000000) {
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	end = .; _end = .; __end = .;
}

