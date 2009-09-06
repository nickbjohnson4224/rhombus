ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xFF100000;

	.mboot : AT(ADDR(.mboot) - 0xFF000000) {
		*(.mboot)
	}

	.text : AT(ADDR(.text) - 0xFF000000) {
		*(.text)
		*(.rodata*)
	}

	.data : AT(ADDR(.data) - 0xFF000000) {
		*(.data)
	}

	.pbss ALIGN (0x1000) : AT(ADDR(.pbss) - 0xFF000000) {
		*(.pbss);
	}

	.pdata ALIGN (0x1000) : AT(ADDR(.pdata) - 0xFF000000) {
		*(.pdata);
	}

	.idata ALIGN (0x1000) : AT(ADDR(.idata) - 0xFF000000) {
		START_OF_INIT = .;
		*(.tdata)
	}

	.itext : AT(ADDR(.itext) - 0xFF000000) {
		*(.ttext)
		. = ALIGN(0x1000);
		END_OF_INIT = .;
	}

	.bss : AT(ADDR(.bss) - 0xFF000000) {
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	end = .; _end = .; __end = .;
}

