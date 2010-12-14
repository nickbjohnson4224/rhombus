ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xF0100000;

	.mboot : AT(ADDR(.mboot) - 0xF0000000) {
		*(.mboot)
	}

	.text : AT(ADDR(.text) - 0xF0000000) {
		*(.text)
		*(.rodata*)
	}

	.data : AT(ADDR(.data) - 0xF0000000) {
		*(.data)
	}

	.pbss ALIGN (0x1000) : AT(ADDR(.pbss) - 0xF0000000) {
		*(.pbss);
	}

	.pdata ALIGN (0x1000) : AT(ADDR(.pdata) - 0xF0000000) {
		*(.pdata);
	}

	.idata ALIGN (0x1000) : AT(ADDR(.idata) - 0xF0000000) {
		START_OF_INIT = .;
		*(.idata)
	}

	.itext : AT(ADDR(.itext) - 0xF0000000) {
		*(.itext)
		. = ALIGN(0x1000);
		END_OF_INIT = .;
	}

	.bss : AT(ADDR(.bss) - 0xF0000000) {
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	end = .; _end = .; __end = .;
}
