/*
 * Copyright 2010 Nick Johnson
 * ISC Licnensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/thread.h>
#include <kernel/io.h>

/****************************************************************************
 * pic_mask
 *
 * This function toggles the masking of individual IRQs sent by the Intel 8259
 * Programmable Interrupt Controller (PIC). Initially, no IRQs are masked.
 */

void pic_mask(uint16_t mask) {
	static uint16_t currmask = 0xFFFF;
	uint8_t smask, mmask;

	/* Toggle bits */
	currmask ^= mask;
	smask = (currmask >> 0) & 0xFF;
	mmask = (currmask >> 8) & 0xFF;

	/* (re)initialize 8259 PIC */
	outb(0x20, 0x11); /* Initialize master */
	outb(0xA0, 0x11); /* Initialize slave */
	outb(0x21, 0x20); /* Master mapped to 0x20 - 0x27 */
	outb(0xA1, 0x28); /* Slave mapped to 0x28 - 0x2E */
	outb(0x21, 0x04); /* Master thingy */
	outb(0xA1, 0x02); /* Slave thingy */
	outb(0x21, 0x01); /* 8086 (standard) mode */
	outb(0xA1, 0x01); /* 8086 (standard) mode */
	outb(0x21, smask); /* Master IRQ mask */
	outb(0xA1, mmask); /* Slave IRQ mask */
}
