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
#include <stdlib.h>

#include <rdi/core.h>
#include <rdi/arch.h>
#include <rdi/io.h>

#include <rho/mutex.h>

#include "ata.h"

struct ata_drive ata[4];

void ata_sleep400(uint8_t drive) {

	inb(ata[drive].base + REG_ASTAT);
	inb(ata[drive].base + REG_ASTAT);
	inb(ata[drive].base + REG_ASTAT);
	inb(ata[drive].base + REG_ASTAT);
}

int ata_wait(uint8_t drive) {
	uint8_t status;
	int i;

	for (i = 0;; i++) {
		status = inb(ata[drive].base + REG_STAT);

		if (!(status & STAT_BUSY)) {
			return 0;
		}

		if (i > 4 && ((status & STAT_ERROR) || (status & STAT_FAULT))) {
			return status;
		}
	}
}

void ata_select(uint8_t drive) {

	outb(ata[drive].base + REG_SELECT, SEL(drive) ? 0xB0 : 0xA0);
	ata_sleep400(drive);
}

void ata_init(void) {
	size_t dr, i;
	uint8_t status, err, cl, ch;
	uint16_t c;
	uint16_t buffer[256];
//	struct robject *robject;

	/* detect I/O ports */
	ata[ATA0].base = 0x1F0;
	ata[ATA1].base = 0x170;
	ata[ATA0].dma  = 0; // FIXME
	ata[ATA1].dma  = 0; // FIXME

	/* detect IRQ numbers */
	ata[ATA0].irq = 14;
	ata[ATA1].irq = 15;

	/* copy controller ports for drives */

	/* ATA 0 master */
	ata[ATA00].base = ata[ATA0].base;
	ata[ATA00].dma  = ata[ATA0].dma;
	ata[ATA00].irq  = ata[ATA0].irq;

	/* ATA 0 slave */
	ata[ATA01].base = ata[ATA0].base;
	ata[ATA01].dma  = ata[ATA0].dma;
	ata[ATA01].irq  = ata[ATA0].irq;

	/* ATA 1 master */
	ata[ATA10].base = ata[ATA1].base;
	ata[ATA10].dma  = ata[ATA1].dma;
	ata[ATA10].irq  = ata[ATA1].irq;

	/* ATA 1 slave */
	ata[ATA11].base = ata[ATA1].base;
	ata[ATA11].dma  = ata[ATA1].dma;
	ata[ATA11].irq  = ata[ATA1].irq;

	/* disable controller IRQs */
	outw(ata[ATA0].base + REG_CTRL, CTRL_NEIN);
	outw(ata[ATA1].base + REG_CTRL, CTRL_NEIN);

	/* detect drives */
	for (dr = 0; dr < 4; dr++) {
		ata_select(dr);

		/* send IDENTIFY command */
		outb(ata[dr].base + REG_COUNT0, 0);
		outb(ata[dr].base + REG_LBA0,   0);
		outb(ata[dr].base + REG_LBA1,   0);
		outb(ata[dr].base + REG_LBA2,   0);
		ata_sleep400(dr);
		outb(ata[dr].base + REG_CMD, CMD_ID);
		ata_sleep400(dr);

		/* read status */
		status = inb(ata[dr].base + REG_STAT);

		/* check for drive response */
		if (!status) continue;

		ata[dr].flags = FLAG_EXIST;

		/* poll for response */
		err = ata_wait(dr);

		/* try to catch ATAPI and SATA devices */
		if (err && (inb(ata[dr].base + REG_LBA1) || inb(ata[dr].base + REG_LBA2))) {
			cl = inb(ata[dr].base + REG_LBA1);
			ch = inb(ata[dr].base + REG_LBA2);
			c = cl | (ch << 8);
			
			ata_sleep400(dr);

			if (c == 0xEB14 || c == 0x9669) {
				/* is ATAPI */
				ata[dr].flags |= FLAG_ATAPI;
				ata[dr].sectsize = 11;

				/* use ATAPI IDENTIFY */
				outb(ata[dr].base + REG_CMD, CMD_ATAPI_ID);
				ata_sleep400(dr);
			}
			else if (c == 0xC33C) {
				/* is SATA */
				ata[dr].flags |= FLAG_SATA;
			}
			else {
				/* unknown device; ignore */
				ata[dr].flags = 0;
				continue;
			}
		}
		else {
			/* assume 512-byte sectors for ATA */
			ata[dr].sectsize = 9;
		}

		/* wait for IDENTIFY to be ready */
		err = ata_wait(dr);

		/* read in IDENTIFY space */
		for (i = 0; i < 256; i++) {
			buffer[i] = inw(ata[dr].base + REG_DATA);
		}

		if (!buffer[ID_TYPE]) {
			/* no drive */
			ata[dr].flags = 0;
			continue;
		}

		ata[dr].signature    = buffer[ID_TYPE];
		ata[dr].capabilities = buffer[ID_CAP];
		ata[dr].commandsets  = buffer[ID_CMDSET] | (uint32_t) buffer[ID_CMDSET+1] << 16;

		/* get LBA mode and disk size */
		if (ata[dr].commandsets & (1 << 26)) {
			/* is LBA48 */
			ata[dr].flags |= FLAG_LONG;
			ata[dr].size   = (uint64_t) buffer[ID_MAX_LBA48+0];
			ata[dr].size  |= (uint64_t) buffer[ID_MAX_LBA48+1] << 16;
			ata[dr].size  |= (uint64_t) buffer[ID_MAX_LBA48+2] << 32;
			ata[dr].size  |= (uint64_t) buffer[ID_MAX_LBA48+3] << 48;
		}
		else {
			/* is LBA24 */
			ata[dr].size  = (uint32_t) buffer[ID_MAX_LBA+0];
			ata[dr].size |= (uint32_t) buffer[ID_MAX_LBA+1] << 16;
		}

		/* get model string */
		for (i = 0; i < 40; i += 2) {
			ata[dr].model[i]   = buffer[ID_MODEL + (i / 2)] >> 8;
			ata[dr].model[i+1] = buffer[ID_MODEL + (i / 2)] & 0xFF;
		}
		ata[dr].model[40] = '\0';
		for (i = 39; i > 0; i--) {
			if (ata[dr].model[i] == ' ') {
				ata[dr].model[i] = '\0';
			}
			else break;
		}

		printf("found drive: ");

		switch (dr) {
		case ATA00: printf("ATA 0 Master\n"); break;
		case ATA01: printf("ATA 0 Slave\n");  break;
		case ATA10: printf("ATA 1 Master\n"); break;
		case ATA11: printf("ATA 1 Slave\n");  break;
		}

		printf("\t");
		printf((ata[dr].flags & FLAG_SATA) ? "S" : "P");
		printf((ata[dr].flags & FLAG_ATAPI) ? "ATAPI " : "ATA ");
		printf((ata[dr].flags & FLAG_LONG) ? "LBA48 " : "LBA24 ");
		printf("\n");

		printf("\tsize: %d KB (%d sectors)\n",
			(uint32_t) ata[dr].size * (1 << ata[dr].sectsize) >> 10,
			(uint32_t) ata[dr].size);
		printf("\tmodel: %s\n", ata[dr].model);


	}
}

size_t ata_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	uint32_t drive;
	uint8_t *buffer2;
	uint32_t buf2off;
	uint64_t sector;
	uint16_t count;
	uint16_t i;

	mutex_spin(&self->driver_mutex);

	drive = (uint32_t) robject_data(self, "drive");

	if (offset >= ata[drive].size << ata[drive].sectsize) {
		return 0;
	}

	if (offset + size > ata[drive].size << ata[drive].sectsize) {
		size = (ata[drive].size << ata[drive].sectsize) - offset;
	}

	buf2off = offset & ((1 << ata[drive].sectsize) - 1);
	count   = ((size + buf2off) >> ata[drive].sectsize);
	buffer2 = malloc(count << ata[drive].sectsize);
	sector  = offset >> ata[drive].sectsize;

	for (i = 0; i < count;) {
		if (count - i > 256) {
			pio_read(drive, sector, 256, (void*) &buffer2[i << ata[drive].sectsize]);
			i += 256;
		}
		else {
			pio_read(drive, sector, count - i, (void*) &buffer2[i << ata[drive].sectsize]);
			break;
		}
	}

	memcpy(buffer, &buffer2[buf2off], size);

	mutex_free(&self->driver_mutex);

	return size;
}

int main(int argc, char **argv) {
	struct robject *robject;
	off_t *disksize;
	uint32_t dr;
	
	rdi_init();
	ata_init();

	for (dr = ATA00; dr <= ATA11; dr++) {

		if (!ata[dr].flags) {
			continue;
		}
		
		robject = rdi_file_cons(robject_new_index(), ACCS_READ);

		disksize = malloc(sizeof(off_t));
		*disksize = ata[dr].size << ata[dr].sectsize;
		robject_set_data(robject, "size", disksize);
		robject_set_data(robject, "drive", (void*) dr);
		robject_set_data(robject, "name", strdup(ata[dr].model));
	}
	
	rdi_global_read_hook = ata_read;

	msendb(RP_CONS(getppid(), 0), ACTION_CHILD);
	_done();

	return 0;
}
