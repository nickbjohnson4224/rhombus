/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

#include <interrupt.h>
#include <process.h>
#include <types.h>

/* IRQ numbering macros *****************************************************/

#define IRQ_INT_BASE 32
#define IRQ_INT_SIZE 16

#define IRQ2INT(n) ((n) + IRQ_INT_BASE)
#define INT2IRQ(n) ((n) - IRQ_INT_BASE)
#define ISIRQ(n) ((n) >= IRQ_INT_BASE && (n) < (IRQ_INT_BASE + IRQ_INT_SIZE))

#define IRQ_NULL 0xFF

/* IRQ redirection **********************************************************/

int   irq_set_redirect(pid_t pid, irqid_t irq);
pid_t irq_get_redirect(irqid_t irq);

/* IRQ masking **************************************************************/

int irq_allow   (irqid_t irq);
int irq_disallow(irqid_t irq);

/* IRQ reset ****************************************************************/

int irq_reset(irqid_t irq);

#endif/*KERNEL_IRQ_H*/
