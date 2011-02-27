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

#ifndef KERNEL_FAULT_H
#define KERNEL_FAULT_H

#include <interrupt.h>
#include <stdint.h>

#define FAULT_DE	0x00	// division by zero
#define FAULT_DB	0x01	// (reserved)
#define FAULT_NI	0x02	// non-maskable interrupt
#define FAULT_BP	0x03	// breakpoint
#define FAULT_OF	0x04	// overflow
#define FAULT_BR	0x05	// bound range exceeded
#define FAULT_UD	0x06	// invalid opcode
#define FAULT_NM	0x07	// no math coprocessor
#define FAULT_DF	0x08	// double fault
#define FAULT_CO	0x09	// coprocessor segment overrun
#define FAULT_TS	0x0A	// invalid TSS
#define FAULT_NP	0x0B	// segment not present
#define FAULT_SS	0x0C	// stack segment fault
#define FAULT_GP	0x0D	// general protection fault
#define FAULT_PF	0x0E	// page fault
#define FAULT_MF	0x10	// math fault
#define FAULT_AC	0x11	// alignment check
#define FAULT_MC	0x12	// machine check
#define FAULT_XM	0x13	// SIMD exception

struct thread *fault_generic(struct thread *image);
struct thread *fault_page   (struct thread *image);
struct thread *fault_float  (struct thread *image);
struct thread *fault_double (struct thread *image);
struct thread *fault_nomath (struct thread *image);
struct thread *fault_gpf    (struct thread *image);

#endif/*KERNEL_FAULT_H*/
