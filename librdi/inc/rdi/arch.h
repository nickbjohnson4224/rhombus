/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef _RDI_ARCH_H
#define _RDI_ARCH_H

#include <stdint.h>
#include <arch.h>
#include <ipc.h>

#if ARCH == X86

/* x86 hardware interface ***************************************************/

	/* port access ****************************************************/

	uint8_t  inb(uint16_t port);
	uint16_t inw(uint16_t port);
	uint32_t ind(uint16_t port);

	void outb(uint16_t port, uint8_t value);
	void outw(uint16_t port, uint16_t value);
	void outd(uint16_t port, uint32_t value);

	void iodelay(uint32_t usec);

	/* IRQ redirection ************************************************/
	
	void rdi_set_irq(uint8_t irq, void (*_irq)(struct msg *msg));
	
	extern void (*_rdi_callback_irq)(struct msg *msg);

	/* virtual 8086 mode **********************************************/

	#define VM86_SP	0x7C00
	#define VM86_SS	0x0000
	#define VM86_IP 0x7C00
	#define VM86_CS 0x0000

	int __vm86(uint32_t ipcs, uint32_t spss);

	// identity maps all of lower memory at 0x00000 through 0x100000
	int vm86_setup(void);

	// loads code at 0x7C00 with a stack at 0x7C00 (calls vm86_setup if needed)
	int vm86_exec (void *code, size_t size);

#endif/*ARCH*/

#endif/*_RDI_ARCH_H*/
