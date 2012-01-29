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

#include <rdi/arch.h>

#include <stdint.h>
#include <stdlib.h>

#include <rho/abi.h>
#include <rho/ipc.h>

/****************************************************************************
 * rdi_set_irq
 *
 * Registers <_irq> as an IRQ callback for IRQ number <irq>. If <_irq> is 
 * null, the IRQs are queued as messages (so it is possible to mwait() for 
 * them).
 */

void rdi_set_irq(uint8_t irq, void (*_irq)(struct msg *msg)) {
	
	/* register IRQ redirect with kernel */
	_rirq(irq);

	/* register IRQ handler */
	when(ACTION_IRQ, _irq);
	_rdi_callback_irq = _irq;
}

void (*_rdi_callback_irq)(struct msg *msg);
