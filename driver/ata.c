/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <driver/ata.h>
#include <driver/pci.h>

#define ATA0_MASTER			0x00
#define ATA0_SLAVE			0x01
#define ATA1_MASTER			0x02
#define ATA1_SLAVE			0x03
#define CHANNEL(d) (((d) >> 1) & 1)
#define SELECTOR(d) ((d) & 1)

#define REG_DATA			0x00000
#define REG_ERROR			0x00001
#define REG_FEATURES		0x00001
#define REG_SECCOUNT0		0x00002
#define REG_LBA0			0x00003
#define REG_LBA1			0x00004
#define REG_LBA2			0x00005
#define REG_HDDEVSEL		0x00006
#define REG_COMMAND			0x00007
#define REG_STATUS			0x00007
#define REG_SECCOUNT1		0x00008
#define REG_LBA3			0x00009
#define REG_LBA4			0x0000A
#define REG_LBA5			0x0000B

#define REG_CONTROL			0x10002
#define REG_ALTSTATUS		0x10002

#define CMD_READ_PIO		0x20
#define CMD_READ_PIO_EXT	0x24
#define CMD_READ_DMA		0xC8
#define CMD_READ_DMA_EXT	0x25
#define CMD_READ_ATAPI		0xA8
#define CMD_WRITE_PIO		0x30
#define CMD_WRITE_PIO_EXT	0x34
#define CMD_WRITE_DMA		0xCA
#define CMD_WRITE_DMA_EXT	0x35
#define CMD_CACHE_FLUSH		0xE7
#define CMD_CACHE_FLUSH_EXT	0xEA
#define CMD_PACKET			0xA0
#define CMD_IDENTIFY_PACKET	0xA1
#define CMD_IDENTIFY		0xEC
#define CMD_EJECT_ATAPI		0x1B

#define STAT_ERROR			0x01
#define STAT_DATAREQ		0x08
#define STAT_SERVICE		0x10
#define STAT_FAULT			0x20
#define STAT_READY			0x40
#define STAT_BUSY			0x80

#define CTRL_NEIN			0x02
#define CTRL_RESET			0x04
#define CTRL_HIGHBYTE		0x80

#define ID_TYPE				0x00
#define ID_CYLINDERS		0x01
#define ID_HEADS			0x03
#define ID_SECTORS			0x06
#define ID_SERIAL			0x0A
#define ID_MODEL			0x1B
#define ID_CAPABILITIES		0x31
#define ID_FIELDVALID		0x35
#define ID_MAX_LBA			0x3C
#define ID_COMMANDSETS		0x52
#define ID_MAX_LBA_EXT		0x64

#define FLAG_EXIST			0x01
#define FLAG_DMA			0x02
#define FLAG_ATAPI			0x04
#define FLAG_LONG			0x08

static void ata_init(device_t selector);
static void ata_halt(void);

/*** Device Information ***/

static device_t ata_controller;

static struct drive {
	uint8_t  flags; /* see FLAG_* */
	uint16_t signature;
	uint16_t capabilities;
	uint16_t commandsets;
	uint32_t size[2];
	char model[41];
} drive[4];

static uint32_t dev;

static uint16_t ata_base[2];
static uint16_t ata_ctrl[2];
static uint16_t ata_dma[2];

/*** Helper Routines ***/

static void ata_out(uint8_t drive, uint32_t reg, uint8_t val) {
	switch (reg >> 16) {
	case 0: outb(ata_base[CHANNEL(drive)] + (reg & 0xFFFF), val); break;
	case 1: outb(ata_ctrl[CHANNEL(drive)] + (reg & 0xFFFF), val); break;
	case 2: outb(ata_dma [CHANNEL(drive)] + (reg & 0xFFFF), val); break;
	}
}

static uint8_t ata_in(uint8_t drive, uint32_t reg) {
	switch (reg >> 16) {
	case 0: return inb(ata_base[CHANNEL(drive)] + (reg & 0xFFFF)); break;
	case 1: return inb(ata_ctrl[CHANNEL(drive)] + (reg & 0xFFFF)); break;
	case 2: return inb(ata_dma [CHANNEL(drive)] + (reg & 0xFFFF)); break;
	}
	return 0xFF;
}

/* guaranteed to take 400 ns, and have no side effects */
static void sleep400(uint8_t drive) {
	inb(ata_ctrl[CHANNEL(drive)] + REG_ALTSTATUS);
	inb(ata_ctrl[CHANNEL(drive)] + REG_ALTSTATUS);
	inb(ata_ctrl[CHANNEL(drive)] + REG_ALTSTATUS);
	inb(ata_ctrl[CHANNEL(drive)] + REG_ALTSTATUS);
}

static void select(uint8_t drive) {
	ata_out(drive, REG_HDDEVSEL, (SELECTOR(drive)) ? 0xB0 : 0xA0);
	sleep400(drive);
}

/*** High Level I/O Routines ***/

static uint8_t *get_block(uint8_t drive, uint32_t off);

/*** Request Handlers ***/

static void ata_read(uint32_t caller, struct request *req) {
	printf("ATA: read ATA%d %s at %x size %x\n", 
		req->resource >> 1, (req->resource & 1) ? "slave" : "master",
		req->fileoff[0], req->datasize);

	req->format  = REQ_WRITE;
	req->dataoff = STDOFF;
	memset(&req->reqdata[req->dataoff - HDRSZ], 'Q', req->datasize);
	tail(caller, SIG_REPLY, req);
}

/*** Driver Interface ***/

struct driver_interface ata = {
	ata_init,
	ata_halt,

	NULL, 0,
};

static void ata_init(device_t dev) {
	size_t i, j;
	uint8_t err, status, cl, ch;
	uint16_t c;
	uint16_t buffer[256];

	printf("ATA: initializing %x:%x.%x\n", dev.bus, dev.slot, dev.sub);

	ata_controller = dev;

	/* detect controller I/O ports */
	ata_base[0] = pci_config_barbase(ata_controller, 0);
	ata_ctrl[0] = pci_config_barbase(ata_controller, 1);
	ata_dma[0]  = pci_config_barbase(ata_controller, 4);
	ata_base[1] = pci_config_barbase(ata_controller, 2);
	ata_ctrl[1] = pci_config_barbase(ata_controller, 3);
	ata_dma[1]  = pci_config_barbase(ata_controller, 4) + 8;

	/* detect and correct for compatibility mode */
	if (!ata_base[0]) ata_base[0] = 0x1F0;
	if (!ata_ctrl[0]) ata_ctrl[0] = 0x3F4;
	if (!ata_base[1]) ata_base[1] = 0x170;
	if (!ata_ctrl[1]) ata_ctrl[1] = 0x374;

	/* reset drives */
	ata_out(ATA0_MASTER, REG_CONTROL, CTRL_RESET);
	ata_out(ATA0_MASTER, REG_CONTROL, 0);
	
	/* disable controller IRQs */
	ata_out(ATA0_MASTER, REG_CONTROL, CTRL_NEIN);
	ata_out(ATA1_MASTER, REG_CONTROL, CTRL_NEIN);


	/* detect ATA drives */
	for (i = 0; i < 4; i++) {
		select(i);
		drive[i].flags = 0;

		/* send IDENTIFY command */
		ata_out(i, REG_COMMAND, CMD_IDENTIFY);
		sleep400(i);

		/* read status */
		status = ata_in(i, REG_STATUS);

		/* check for drive response */
		if (!status) {
			continue;
		}
		else {
			drive[i].flags |= FLAG_EXIST;
		}

		/* poll for response */
		while (1) {
			status = ata_in(i, REG_STATUS);
			if (status & STAT_ERROR) {
				err = 1;
				break;
			}
			if (!(status & STAT_BUSY) && (status & STAT_DATAREQ)) {
				err = 0;
				break;
			}
		}

		/* try to catch ATAPI devices */
		if (err) {
			cl = ata_in(i, REG_LBA1);
			ch = ata_in(i, REG_LBA2);
			c = cl | (ch << 8);

			if (c == 0xEB14 || c == 0x9669) {
				drive[i].flags |= FLAG_ATAPI;
			}
			else {
				drive[i].flags = 0;
				continue;
			}

			ata_out(i, REG_COMMAND, CMD_IDENTIFY_PACKET);
			sleep400(i);
		}

		/* read in IDENTIFY space */
		for (j = 0; j < 256; j++) {
			buffer[j] = inw(ata_base[CHANNEL(i)] + REG_DATA);
		}

		drive[i].signature    = *((uint16_t*) &buffer[ID_TYPE]);
		drive[i].capabilities = *((uint16_t*) &buffer[ID_CAPABILITIES]);
		drive[i].commandsets  = *((uint32_t*) &buffer[ID_COMMANDSETS]);

		if (drive[i].commandsets & (1 << 26)) {
			drive[i].flags |= FLAG_LONG;
			drive[i].size[0] = *((uint32_t*) &buffer[ID_MAX_LBA_EXT]);
			drive[i].size[1] = *((uint16_t*) &buffer[ID_MAX_LBA_EXT + 4]);
		}
		else {
			drive[i].size[0] = *((uint32_t*) &buffer[ID_MAX_LBA]);
			drive[i].size[1] = 0;
		}

		for (j = 0; j < 40; j += 2) {
			drive[i].model[j]   = buffer[ID_MODEL + (j / 2)] >> 8;
			drive[i].model[j+1] = buffer[ID_MODEL + (j / 2)] & 0xFF;
		}
		drive[i].model[40] = '\0';

		printf("found drive %d: ", i);
		printf((drive[i].flags & FLAG_LONG) ? "LBA48 " : "LBA28 ");
		printf((drive[i].flags & FLAG_ATAPI) ? "ATAPI " : "ATA ");
		printf("model %s ", drive[i].model);
		printf("\n");
	}

	/* register interface handler */
	sigregister(SIG_READ, ata_read);
}

static void ata_halt(void) {
	return;
}
