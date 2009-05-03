ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xF8100000;

	.text : AT(ADDR(.text) - 0xF8000000) {
		*(.text)
		*(.rodata*)
	}

	.data : AT(ADDR(.data) - 0xF8000000) {
		*(.data)
	}

	.pdata ALIGN (0x1000) : AT(ADDR(.pdata) - 0xF8000000) {
		*(.pdata)
	}

	.tdata ALIGN (0x1000) : AT(ADDR(.tdata) - 0xF8000000) {
		*(.tdata)
	}

	.ttext : AT(ADDR(.ttext) - 0xF8000000) {
		*(.ttext)
	}

	.bss : AT(ADDR(.bss) - 0xF8000000) {
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	end = .; _end = .; __end = .;
}

