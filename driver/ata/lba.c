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

#include <stdint.h>
#include <string.h>

#include <rdi/core.h>
#include <rdi/arch.h>

#include "ata.h"

int ata_send_lba(uint8_t drive, uint64_t sector) {
	uint8_t lba[8];
	uint8_t head;
	int lba48;

	ata_select(drive);

	/* format LBA bytes from sector index */
	if (sector & 0xFFFFFFFFF0000000ULL && (ata[drive].flags & FLAG_LONG)) {
		/* use LBA48 */
		lba48 = 1;
		lba[5] = (sector >> 40) & 0xFF;
		lba[4] = (sector >> 32) & 0xFF;
		lba[3] = (sector >> 24) & 0xFF;
		head   = 0;
	}
	else {
		/* use LBA28 */
		lba48  = 0;
		head   = (sector >> 24) & 0xF;
	}
	lba[2] = (sector >> 16) & 0xFF;
	lba[1] = (sector >> 8)  & 0xFF;
	lba[0] = (sector >> 0)  & 0xFF;

	printf("pies %x\n", inb(ata[drive].base + REG_STAT));

	/* wait for drive to be ready */
	if (ata_wait(drive)) return -1;

	printf("lies\n");

	/* send LBA */
	outb(ata[drive].base + REG_SELECT, (SEL(drive) ? 0xF0 : 0xE0) | head);
	ata_sleep400(drive);
	outb(ata[drive].base + REG_COUNT0, 0x01);
	outb(ata[drive].base + REG_LBA0, lba[0]);
	outb(ata[drive].base + REG_LBA1, lba[1]);
	outb(ata[drive].base + REG_LBA2, lba[2]);
	if (lba48) {
		outb(ata[drive].base + REG_LBA3, lba[3]);
		outb(ata[drive].base + REG_LBA4, lba[4]);
		outb(ata[drive].base + REG_LBA5, lba[5]);
	}
	
	return lba48;
}
