/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#ifndef ARCH_H
#define ARCH_H

/* Address space information ******************************************/

#define PAGESZ 0x1000

/* Address space layout ***********************************************/

#define EXEC_START	0x00000000
#define HEAP_START	0x10000000
#define HEAP_MXBRK	0x20000000
#define BOOT_IMAGE	0xD0000000
#define FD_TABLE    0xE0000000
#define ARGV_TABLE	0xE0010000
#define INFO_TABLE	0xE8000000

#define TMP_DST     0xFF000000
#define TMP_SRC     0xFF010000
#define TMP_MAP     0xFF800000
#define PGE_MAP     0xFFC00000

#define KERNEL_HEAP     (KSPACE + 0x1000000)
#define KERNEL_HEAP_END (KSPACE + 0x8000000)

/** These must remain in order **/
#define SSPACE	0x80000000	/* Stack space */
#define ESPACE	0xC0000000	/* Exec persistent data */
#define KSPACE 	0xF0000000	/* Kernel space */

#endif/*ARCH_H*/
