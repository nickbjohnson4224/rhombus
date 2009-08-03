ENTRY(main)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
  . = 0xF8001000;
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}

