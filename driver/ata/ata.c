/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/driver.h>
#include <flux/signal.h>
#include <flux/request.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <driver/ata.h>
#include <driver/pci.h>

static void ata_init(device_t selector);
static void ata_halt(void);

/*** Device Information ***/

device_t ata_controller;

struct ata_drive ata_drive[4];

/* I/O port BARs and IRQs */
uint16_t ata_base[4];
uint16_t ata_ctrl[4];
uint16_t ata_dma [4];
uint8_t  ata_irq [4];

/*** Helper Routines ***/

/* guaranteed to take 400 ns, and have no side effects */
void ata_sleep400(uint8_t drive) {
	inb(ata_ctrl[drive] + REG_ASTAT);
	inb(ata_ctrl[drive] + REG_ASTAT);
	inb(ata_ctrl[drive] + REG_ASTAT);
	inb(ata_ctrl[drive] + REG_ASTAT);
}

/* select drive command */
void ata_select(uint8_t drive) {
	outb(ata_base[drive] + REG_SELECT, SEL(drive) ? 0xB0 : 0xA0);
	ata_sleep400(drive);
}

/*** Request Handlers ***/

static void ata_read(uint32_t caller, req_t *req) {
	uint32_t sector, size, offset;
	uint8_t dr;

	/* reject bad requests */
	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req_cksum(req));
	}

	/* calculate command */
	dr = req->resource;
	offset = req->fileoff & ((1 << ata_drive[dr].sectsize) - 1);
	sector = req->fileoff >> ata_drive[dr].sectsize;
	size = (req->datasize + offset > (1 << ata_drive[dr].sectsize)) 
		? (1 << ata_drive[dr].sectsize) - offset
		: req->datasize;

	/* reject requests to nonexistent drives */
	if (dr > 4 || !(ata_drive[dr].flags & FLAG_EXIST)) {
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req_cksum(req));
	}

	/* do read command */
	req_setbuf(req, STDOFF, size);
	pio_read_sector(dr, sector, (uint16_t*) req_getbuf(req));
	req_setbuf(req, STDOFF + offset, size);

	/* send reply */
	req->format = REQ_WRITE;
	tail(caller, SIG_REPLY, req_cksum(req));
}

static void ata_write(uint32_t caller, struct request *req) {
	uint32_t sector, size, offset;
	uint8_t *buffer;
	uint8_t dr;

	/* reject bad requests */
	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req_cksum(req));
	}

	/* calculate command */
	dr = req->resource;
	offset = req->fileoff & ((1 << ata_drive[dr].sectsize) - 1);
	sector = req->fileoff >> ata_drive[dr].sectsize;
	size = (req->datasize + offset > ata_drive[dr].sectsize) 
		? ata_drive[dr].sectsize - offset
		: req->datasize;

	/* reject requests to nonexistent drives */
	if (!(ata_drive[dr].flags & FLAG_EXIST)) {
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req_cksum(req));
	}

	/* do write command */
	buffer = malloc(ata_drive[dr].sectsize);
	pio_read_sector (dr, sector, (uint16_t*) buffer);
	memcpy(&buffer[offset], req_getbuf(req), size);
	pio_write_sector(dr, sector, (uint16_t*) buffer);
	free(buffer);

	/* send reply */
	req->format   = REQ_READ;
	req->datasize = size;
	tail(caller, SIG_REPLY, req_cksum(req));
}

/*** Driver Interface ***/

struct driver_interface ata = {
	ata_init,
	ata_halt,

	NULL, 0,
};

static void ata_init(device_t dev) {
	char *secbuffer;
	size_t dr, i;
	uint8_t err, status, cl, ch;
	uint16_t c;
	uint16_t buffer[256];

	printf("ATA: initializing %x:%x.%x\n", dev.bus, dev.slot, dev.sub);

	ata_controller = dev;

	/* detect controller I/O ports */
	ata_base[ATA0] = pci_config_barbase(ata_controller, 0);
	ata_ctrl[ATA0] = pci_config_barbase(ata_controller, 1);
	ata_dma [ATA0] = pci_config_barbase(ata_controller, 4);
	ata_irq [ATA0] = 14;
	ata_base[ATA1] = pci_config_barbase(ata_controller, 2);
	ata_ctrl[ATA1] = pci_config_barbase(ata_controller, 3);
	ata_dma [ATA1] = pci_config_barbase(ata_controller, 4) + 8;
	ata_irq [ATA1] = 15;

	/* detect and correct for compatibility mode */
	if (!ata_base[ATA0]) ata_base[ATA00] = 0x1F0;
	if (!ata_ctrl[ATA0]) ata_ctrl[ATA00] = 0x3F4;
	if (!ata_base[ATA1]) ata_base[ATA10] = 0x170;
	if (!ata_ctrl[ATA1]) ata_ctrl[ATA10] = 0x374;

	/* copy controller ports for drives */

	/* ATA 0 Master */
	ata_base[ATA00] = ata_base[ATA0];
	ata_ctrl[ATA00] = ata_ctrl[ATA0];
	ata_dma [ATA00] = ata_dma [ATA0];
	ata_irq [ATA00] = ata_irq [ATA0];

	/* ATA 0 Slave */
	ata_base[ATA01] = ata_base[ATA0];
	ata_ctrl[ATA01] = ata_ctrl[ATA0];
	ata_dma [ATA01] = ata_dma [ATA0];
	ata_irq [ATA01] = ata_irq [ATA0];

	/* ATA 1 Master */
	ata_base[ATA10] = ata_base[ATA1];
	ata_ctrl[ATA10] = ata_ctrl[ATA1];
	ata_dma [ATA10] = ata_dma [ATA1];
	ata_irq [ATA10] = ata_irq [ATA1];

	/* ATA 1 Slave */
	ata_base[ATA11] = ata_base[ATA1];
	ata_ctrl[ATA11] = ata_ctrl[ATA1];
	ata_dma [ATA11] = ata_dma [ATA1];
	ata_irq [ATA11] = ata_irq [ATA1];

	/* disable controller IRQs */
	outb(ata_base[ATA0] + REG_CTRL, CTRL_NEIN);
	outb(ata_base[ATA1] + REG_CTRL, CTRL_NEIN);

	/* detect ATA drives */
	for (dr = 0; dr < 4; dr++) {
		ata_select(dr);
		ata_drive[dr].flags = 0;

		/* send IDENTIFY command */
		outb(ata_base[dr] + REG_CMD, CMD_ID);
		ata_sleep400(dr);

		/* read status */
		status = inb(ata_base[dr] + REG_STAT);

		/* check for drive response */
		if (!status) {
			continue;
		}
		else {
			ata_drive[dr].flags |= FLAG_EXIST;
		}

		/* poll for response */
		while (1) {
			status = inb(ata_base[dr] + REG_STAT);
			if (status & STAT_ERROR) {
				err = 1;
				break;
			}
			if (!(status & STAT_BUSY) && (status & STAT_DRQ)) {
				err = 0;
				break;
			}
		}

		/* try to catch ATAPI and SATA devices */
		if (err) {
			cl = inb(ata_base[dr] + REG_LBA1);
			ch = inb(ata_base[dr] + REG_LBA2);
			c = cl | (ch << 8);

			if (0) ata_sleep400(dr);
#ifdef ATACONF_ATAPI
			if (c == 0xEB14 || c == 0x9669) {
				ata_drive[dr].flags |= FLAG_ATAPI;
				ata_drive[dr].sectsize = 11;

				/* use packet-based IDENTIFY */
				outb(ata_base[dr] + REG_CMD, CMD_ID_ATAPI);
				ata_sleep400(dr);
			}
#endif
#ifdef ATACONF_SATA
			if (c == 0xC33C) {
				ata_drive[dr].flags |= FLAG_SATA;
			}
#endif
			else {
				/* unknown device - exit */
				ata_drive[dr].flags = 0;
				continue;
			}
		}
		else {
			ata_drive[dr].sectsize = 9;
		}

		/* read in IDENTIFY space */
		for (i = 0; i < 256; i++) {
			buffer[i] = inw(ata_base[dr] + REG_DATA);
		}

		ata_drive[dr].signature    = *((uint16_t*) &buffer[ID_TYPE]);
		ata_drive[dr].capabilities = *((uint16_t*) &buffer[ID_CAP]);
		ata_drive[dr].commandsets  = *((uint32_t*) &buffer[ID_CMDSET]);

#ifdef ATACONF_LONG
		/* get LBA mode and disk size */
		if (ata_drive[dr].commandsets & (1 << 26)) {
			ata_drive[dr].flags |= FLAG_LONG;
			ata_drive[dr].size = *((uint64_t*) &buffer[ID_MAX_LBA48]);
		}
		else {
			ata_drive[dr].size = *((uint32_t*) &buffer[ID_MAX_LBA]);
		}
#else
		ata_drive[dr].size = *((uint32_t*) &buffer[ID_MAX_LBA]);
#endif	

		/* get model string and null-terminate */
		for (i = 0; i < 40; i += 2) {
			ata_drive[dr].model[i]   = buffer[ID_MODEL + (i / 2)] >> 8;
			ata_drive[dr].model[i+1] = buffer[ID_MODEL + (i / 2)] & 0xFF;
		}
		ata_drive[dr].model[40] = '\0';

		/* remove trailing spaces from model string */
		for (i = 39; i; i--) {
			if (ata_drive[dr].model[i] == ' ') {
				ata_drive[dr].model[i] = '\0';
			}
			else {
				break;
			}
		}

#ifdef ATACONF_DMA
		/* detect DMA capability */
		status = inb(ata_dma[dr] + REG_DMA_STAT);
		if ((SEL(dr) == MASTER) && (status & DMA_STAT_MDMA)) {
			ata_drive[dr].flags |= FLAG_DMA;
		}
		if ((SEL(dr) == SLAVE)  && (status & DMA_STAT_SDMA)) {
			ata_drive[dr].flags |= FLAG_DMA;
		}
#endif

		printf("found drive: ");

		switch (dr) {
		case ATA00: printf("ATA 0 Master\n"); break;
		case ATA01: printf("ATA 0 Slave\n");  break;
		case ATA10: printf("ATA 1 Master\n"); break;
		case ATA11: printf("ATA 1 Slave\n");  break;
		}

		printf("\t");
		printf((ata_drive[dr].flags & FLAG_SATA) ? "S" : "P");
		printf((ata_drive[dr].flags & FLAG_ATAPI) ? "ATAPI " : "ATA ");
		printf((ata_drive[dr].flags & FLAG_LONG) ? "LBA48 " : "LBA28 ");
		printf((ata_drive[dr].flags & FLAG_DMA) ? "DMA " : "");
		printf("\n");

		printf("\tsize: %d KB (%d sectors)\n", 
			(uint32_t) ata_drive[dr].size * (1 << ata_drive[dr].sectsize) >> 10,
			(uint32_t) ata_drive[dr].size);
		printf("\tmodel: %s\n", ata_drive[dr].model);
	}
	
	/* register interface handler */
	sigregister(SIG_READ,  ata_read);
	sigregister(SIG_WRITE, ata_write);
}

static void ata_halt(void) {
	return;
}
