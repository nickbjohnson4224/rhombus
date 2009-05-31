ENTRY(main)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
  . = 0xF6000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
  . = 0xF6001000;
  .text : { *(.text) }
}

