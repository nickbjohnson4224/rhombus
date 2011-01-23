/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ARCH_H
#define ARCH_H

/* architecture *******************************************************/

#define ARCH	X86
#define BITS	32

/* address space information ******************************************/

#if ARCH == X86
	#define PAGESZ 0x1000
#endif

/* address space layout ***********************************************/

#if ARCH == X86
	#define EXEC_START	0x00010000
	#define HEAP_START	0x10000000
	#define HEAP_MXBRK	0x11000000
	#define HEAP2_START	0x20000000
	#define HEAP2_MXBRK	0x21000000
	#define BOOT_IMAGE	0x40000000

	#define DL_BASE		0xC0000000
	#define DL_HEAP		0xC2000000

	#define TMP_DST     0xFF000000
	#define TMP_SRC     0xFF010000
	#define TMP_MAP     0xFF800000
	#define PGE_MAP     0xFFC00000

	#define KERNEL_HEAP     (KSPACE + 0x01000000)
	#define KERNEL_HEAP_END (KSPACE + 0x08000000)

	/* physical address of emergency frames */
	#define BOOT_POOL	0x00400000

	/** These must remain in order **/
	#define SSPACE	0x80000000	/* Stack space */
	#define ESPACE	0xC0000000	/* Exec persistent data */
	#define KSPACE 	0xF0000000	/* Kernel space */
#endif

#endif/*ARCH_H*/
