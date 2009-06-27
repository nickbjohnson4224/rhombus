ENTRY(main)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
  . = 0xF8000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
  . = 0xFC000000;
  .text : { *(.text) }
}

