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

#ifndef KERNEL_CPU_H
#define KERNEL_CPU_H

#include <stdint.h>
#include <stdbool.h>

/* general CPU operations ***************************************************/

void cpu_set_stack(void *ptr);
void cpu_idle(void);
void cpu_halt(void);

/* floating point unit operations *******************************************/

void fpu_save(void *fxdata);
void fpu_load(void *fxdata);
void cpu_init_fpu(void);

/* x86 specific operations **************************************************/

void cpu_set_cr0   (uint32_t value);
void cpu_set_cr1   (uint32_t value);
void cpu_set_cr2   (uint32_t value);
void cpu_set_cr3   (uint32_t value);
void cpu_set_eflags(uint32_t value);

uint32_t cpu_get_cr0   (void);
uint32_t cpu_get_cr1   (void);
uint32_t cpu_get_cr2   (void);
uint32_t cpu_get_cr3   (void);
uint32_t cpu_get_eflags(void);
uint32_t cpu_get_id    (uint32_t selector);
uint32_t cpu_get_tsc   (void);

void cpu_set_ts(void);
void cpu_clr_ts(void);
bool cpu_tst_ts(void);

void cpu_flush_tlb_full(void);
void cpu_flush_tlb_part(uint32_t page);

struct tss {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t unused[15];
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldt;
	uint16_t trap, iomap_base;
} __attribute__ ((packed));

void cpu_sync_tss(void);

struct idt {
	uint16_t base_l;
	uint16_t seg;
	uint8_t reserved;
	uint8_t flags;
	uint16_t base_h;
} __attribute__ ((packed));

void cpu_set_idt(struct idt *idt);

/* SMP (to be implemented) **************************************************/

typedef uint8_t cpuid_t;

void cpu_switch(cpuid_t cpu);

#endif/*KERNEL_CPU_H*/
