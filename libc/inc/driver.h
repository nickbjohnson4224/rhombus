/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef DRIVER_H
#define DRIVER_H

#include <natio.h>

/* I/O request wrapper ******************************************************/

int di_wrap_read (size_t   (*di_read) (uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off));
int di_wrap_write(size_t   (*di_write)(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off));
int di_wrap_reset(int      (*di_reset)(uint64_t source, uint32_t index));
int di_wrap_sync (int      (*di_sync) (uint64_t source, uint32_t index));
int di_wrap_size (uint64_t (*di_size) (uint64_t source, uint32_t index));
int di_wrap_share(int      (*di_share)(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off));

void __read_wrapper (struct msg *msg);
void __write_wrapper(struct msg *msg);
void __sync_wrapper (struct msg *msg);
void __reset_wrapper(struct msg *msg);
void __share_wrapper(struct msg *msg);

extern size_t   (*_di_read) (uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off);
extern size_t   (*_di_write)(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off);
extern int      (*_di_reset)(uint64_t source, uint32_t index);
extern int      (*_di_sync) (uint64_t source, uint32_t index);
extern uint64_t (*_di_size) (uint64_t source, uint32_t index);
extern int      (*_di_share)(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t off);

/* port access **************************************************************/

uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/* IRQ redirection **********************************************************/

int di_wrap_irq(uint8_t irq, void (*irq_handler)(struct msg *msg));

/* virtual 8086 mode ********************************************************/

#define VM86_SP	0x7C00
#define VM86_SS	0x0000
#define VM86_IP 0x7C00
#define VM86_CS 0x0000

int __vm86(uint32_t ipcs, uint32_t spss);

// identity maps all of lower memory at 0x00000 through 0x100000
int vm86_setup(void);

// loads code at 0x7C00 with a stack at 0x7C00 (calls vm86_setup if needed)
int vm86_exec (void *code, size_t size);

#endif/*DRIVER_H*/
