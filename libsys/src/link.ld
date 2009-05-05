OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xF4000000;

	.text : { *(.text) }
	.data : { *(.data) }
	.bss :  { *(.bss) }
	end = .; _end = .; __end = .;
}

