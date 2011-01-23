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

#include <thread.h>
#include <debug.h>
#include <space.h>
#include <ports.h>
#include <irq.h>

/* IRQ redirection **********************************************************/

/*****************************************************************************
 * irq_holder
 *
 * Array of pids of processes to send IRQs to. Modified by irq_set_redirect
 * only.
 */

static pid_t irq_holder[IRQ_INT_SIZE];

/*****************************************************************************
 * irq_redirect (interrupt handler)
 *
 * Sends a message to PORT_IRQ of the process registered to handle the calling
 * IRQ by irq_set_redirect.
 */

struct thread *irq_redirect(struct thread *image) {
	return thread_send(NULL, irq_holder[INT2IRQ(image->num)], PORT_IRQ, NULL);
}

/*****************************************************************************
 * irq_set_redirect
 *
 * Sets the process with id <pid> to be sent a message on reception of the
 * IRQ <irq>. If <pid> is zero, no process is sent a message. Returns zero on
 * success, nonzero on failure.
 */

int irq_set_redirect(pid_t pid, irqid_t irq) {

	if (irq > IRQ_INT_SIZE) {
		return 1;
	}

	irq_holder[irq] = pid;
	int_set_handler(IRQ2INT(irq), irq_redirect);
	irq_allow(irq);

	return 0;
}

/*****************************************************************************
 * irq_get_redirect
 *
 * Returns the pid of the process that will be sent a message on reception of
 * the IRQ <irq>. Returns zero if no process is registered to recieve that
 * IRQ.
 */

pid_t irq_get_redirect(irqid_t irq) {

	if (irq > IRQ_INT_SIZE) {
		return 1;
	}

	return irq_holder[irq];
}

/* 8259 PIC driver **********************************************************/

/*****************************************************************************
 * pic_*_mask
 *
 * These masks have bits set corresponding to the IRQs that are _disabled_ on
 * the PIC.
 *
 * Note: pic_master_mask always has 0x4 unset (i.e. IRQ2 is always allowed.) 
 * This is because all slave-generated IRQs are redirected through IRQ2, so
 * if it is masked, all slave IRQs (like ones from the ATA controller) are
 * masked regardless of pic_slave_mask.
 */

static uint8_t pic_master_mask = 0xFB;
static uint8_t pic_slave_mask  = 0xFF;

/*****************************************************************************
 * pic_sync_mask
 *
 * Reset the PIC and set its masks to pic_*_mask.
 */

static void pic_sync_mask(void) {

	/* (re)initialize 8259 PIC */
	outb(0x20, 0x11); /* Initialize master */
	outb(0xA0, 0x11); /* Initialize slave */
	outb(0x21, 0x20); /* Master mapped to 0x20 - 0x27 */
	outb(0xA1, 0x28); /* Slave mapped to 0x28 - 0x2E */
	outb(0x21, 0x04); /* Master thingy */
	outb(0xA1, 0x02); /* Slave thingy */
	outb(0x21, 0x01); /* 8086 (standard) mode */
	outb(0xA1, 0x01); /* 8086 (standard) mode */
	outb(0x21, pic_master_mask); /* Master IRQ mask */
	outb(0xA1, pic_slave_mask);  /* Slave IRQ mask */
}

/*****************************************************************************
 * irq_allow
 *
 * Makes sure the IRQ <irq> can be recieved. Returns zero on success, nonzero
 * on failure.
 */

int irq_allow(irqid_t irq) {
	
	if (irq > 16) {
		return 1;
	}

	if (irq < 8) {
		pic_master_mask &= ~(1 << irq);
	}
	else {
		irq -= 8;
		pic_slave_mask &= ~(1 << irq);
	}

	pic_sync_mask();

	return 0;
}

/*****************************************************************************
 * irq_disallow
 *
 * Makes sure the IRQ <irq> cannot be recieved. Returns zero on success,
 * nonzero on failure.
 */

int irq_disallow(irqid_t irq) {
	
	if (irq > 16) {
		return 1;
	}

	if (irq < 8) {
		pic_master_mask |= (1 << irq);
	}
	else {
		irq -= 8;
		pic_slave_mask |= (1 << irq);
	}

	pic_sync_mask();

	return 0;
}

/*****************************************************************************
 * irq_reset
 *
 * Resets IRQs after an IRQ is handled. <irq> is the specific IRQ that was
 * handled. Assumes that there actually was a valid IRQ generated. Returns
 * zero on success, nonzero on failure.
 *
 * Note: this exists mostly to cater to the 8259 PIC's requirements. After it
 * generates an IRQ, it has to be reset, and the way it needs to be reset
 * depends on which IRQ was generated. This function may not need <irq> or may
 * not be needed at all on non-PC systems or systems with an APIC.
 */

int irq_reset(irqid_t irq) {
	
	if (irq > IRQ_INT_SIZE) {
		return 1;
	}

	if (irq > 8) {
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);

	return 0;
}
