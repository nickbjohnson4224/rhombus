/* Khaos default linker script */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0x400000;

	_stext = .;
	.text  : { *(.text) }
	_etext = .;

	_sxtext = .;
	.xtext : { *(.xtext) }
	_extext = .;

	_sdata = .;
	.data  : { *(.data) }
	_edata = .;

	_sxdata = .;
	.xdata : { *(.xdata) }
	_exdata = .;
	
	_sbss = .;
	.bss   : { *(.bss)  }
	_ebss = .;

}
