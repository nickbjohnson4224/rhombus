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

#include <ktime.h>

#define IRQ(n) (n + 32)
#define DEIRQ(n) (n - 32)

typedef uint8_t irqid_t;

int   irq_set_redirect(pid_t pid, irqid_t irq);
pid_t irq_get_redirect(irqid_t irq);

int irq_allow   (irqid_t irq);
int irq_disallow(irqid_t irq);

#endif/*KERNEL_IRQ_H*/
