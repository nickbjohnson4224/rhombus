/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>
#include <stdlib.h>
#include <signal.h>
#include <mmap.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <driver/ata.h>
#include <driver/pci.h>

#define SECTSIZE			0x200

#define ATA0				0x00
#define ATA1				0x02

#define ATA00				0x00
#define ATA01				0x01
#define ATA10				0x02
#define ATA11				0x03

#define SEL(d) ((d) & 1)
#define MASTER				0
#define SLAVE				1

#define REG_DATA			0x00
#define REG_ERR				0x01
#define REG_FEATURE			0x01
#define REG_COUNT0			0x02
#define REG_LBA0			0x03
#define REG_LBA1			0x04
#define REG_LBA2			0x05
#define REG_SELECT			0x06
#define REG_CMD				0x07
#define REG_STAT			0x07
#define REG_COUNT1			0x08
#define REG_LBA3			0x09
#define REG_LBA4			0x0A
#define REG_LBA5			0x0B

#define REG_CTRL			0x02
#define REG_ASTAT			0x02

#define REG_DMA_CMD			0x00
#define REG_DMA_STAT		0x02
#define REG_DMA_ADDR		0x04

#define CMD_READ_PIO		0x20
#define CMD_READ_PIO48		0x24
#define CMD_READ_DMA		0xC8
#define CMD_READ_DMA48		0x25
#define CMD_READ_ATAPI		0xA8
#define CMD_WRITE_PIO		0x30
#define CMD_WRITE_PIO48		0x34
#define CMD_WRITE_DMA		0xCA
#define CMD_WRITE_DMA48		0x35
#define CMD_CACHE_FLUSH		0xE7
#define CMD_CACHE_FLUSH48	0xEA
#define CMD_ID				0xEC
#define CMD_ID_ATAPI		0xA1
#define CMD_EJECT_ATAPI		0x1B
#define CMD_ATAPI			0xA0

#define STAT_ERROR			0x01
#define STAT_DRQ			0x08
#define STAT_SERVICE		0x10
#define STAT_FAULT			0x20
#define STAT_READY			0x40
#define STAT_BUSY			0x80

#define CTRL_NEIN			0x02
#define CTRL_RESET			0x04
#define CTRL_HBYTE			0x80

#define ID_TYPE				0x00
#define ID_SERIAL			0x0A
#define ID_MODEL			0x1B
#define ID_CAP				0x31
#define ID_VALID			0x35
#define ID_MAX_LBA			0x3C
#define ID_CMDSET			0x52
#define ID_MAX_LBA48		0x64

#define DMA_CMD_RUN			0x01
#define DMA_CMD_RW			0x08
#define DMA_STAT_READY		0x01
#define DMA_STAT_ERROR		0x02
#define DMA_STAT_IRQ		0x04
#define DMA_STAT_MDMA		0x20
#define DMA_STAT_SDMA		0x40

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
	char     model[41];
} drive[4];

static uint32_t dev;

static uint16_t ata_base[4];
static uint16_t ata_ctrl[4];
static uint16_t ata_dma [4];
static uint8_t  ata_irq [4];

/*** Helper Routines ***/

/* guaranteed to take 400 ns, and have no side effects */
static void sleep400(uint8_t drive) {
	inb(ata_ctrl[drive] + REG_ASTAT);
	inb(ata_ctrl[drive] + REG_ASTAT);
	inb(ata_ctrl[drive] + REG_ASTAT);
	inb(ata_ctrl[drive] + REG_ASTAT);
}

/* select drive command */
static void select(uint8_t drive) {
	outb(ata_base[drive] + REG_SELECT, SEL(drive) ? 0xB0 : 0xA0);
	sleep400(drive);
}

/*** PIO ***/

static void pio_read_sector(uint8_t drive, uint32_t sector, uint16_t *buffer) {
	size_t i;
	uint8_t err;
	uint8_t lba[3], head;

	lba[0] = (sector >> 0)  & 0xFF;
	lba[1] = (sector >> 8)  & 0xFF;
	lba[2] = (sector >> 16) & 0xFF;
	head   = (sector >> 24) & 0xF;

	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	outb(ata_base[drive] + REG_SELECT, (SEL(drive) ? 0xF0 : 0xE0) | head);
	outb(ata_base[drive] + REG_COUNT0, 0x01);
	outb(ata_base[drive] + REG_LBA0, lba[0]);
	outb(ata_base[drive] + REG_LBA1, lba[1]);
	outb(ata_base[drive] + REG_LBA2, lba[2]);
	outb(ata_base[drive] + REG_CMD, CMD_READ_PIO);

	for (i = 0; i < SECTSIZE / sizeof(uint16_t); i++) {
		sleep400(drive);
		while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
		buffer[i] = inw(ata_base[drive] + REG_DATA);
	}
}

static void pio_write_sector(uint8_t drive, uint32_t sector, uint16_t *buffer) {
	size_t i;
	uint8_t err;
	uint8_t lba[3], head;

	lba[0] = (sector >> 0)  & 0xFF;
	lba[1] = (sector >> 8)  & 0xFF;
	lba[2] = (sector >> 16) & 0xFF;
	head   = (sector >> 24) & 0xF;

	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	outb(ata_base[drive] + REG_SELECT, (SEL(drive) ? 0xF0 : 0xE0) | head);
	outb(ata_base[drive] + REG_COUNT0, 0x01);
	outb(ata_base[drive] + REG_LBA0, lba[0]);
	outb(ata_base[drive] + REG_LBA1, lba[1]);
	outb(ata_base[drive] + REG_LBA2, lba[2]);
	outb(ata_base[drive] + REG_CMD, CMD_WRITE_PIO);

	for (i = 0; i < SECTSIZE / sizeof(uint16_t); i++) {
		sleep400(drive);
		while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
		outw(ata_base[drive] + REG_DATA, buffer[i]);
	}

	outb(ata_base[drive] + REG_CMD, CMD_CACHE_FLUSH);
	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
}

/*** DMA ***/

/*static uint32_t *ata_prdt[4];
static uintptr_t ata_prdt_phys[4];
static uint8_t  *ata_buff[4];
static uintptr_t ata_buff_phys[4];

static void dma_init(void) {
	int i;

	ata_prdt[0]      = malloc(0x1000);
	ata_prdt_phys[0] = pmap(ata_prdt[0], PROT_READ | PROT_WRITE);
	ata_buff[0]      = malloc(0x1000);
	ata_buff_phys[0] = pmap(ata_buff[0], PROT_READ | PROT_WRITE);

	for (i = 1; i < 4; i++) {
		ata_prdt[i] = &ata_prdt[0][i * BLOCKSIZE / sizeof(uint32_t)];
		ata_prdt_phys[i] = ata_prdt_phys[0] + (i * BLOCKSIZE);
		ata_buff[i] = &ata_buff[0][i * BLOCKSIZE / sizeof(uint8_t)];
		ata_buff_phys[i] = ata_buff_phys[0] + (i * BLOCKSIZE);

		ata_prdt[i][0] = ata_buff_phys[i];
		ata_prdt[i][1] = BLOCKSIZE | (0x1000 << 16);

		if (drive[i].flags & FLAG_EXIST) {
			if (drive[i].flags & FLAG_ATAPI) {
				select(i);
				outb(ata_base[i] + REG_FEATURE, 0x01);
			}
		}
	}
}

static void dma_read_sector28(uint8_t drive, uint32_t sector);
static void dma_write_sector28(uint8_t drive, uint32_t sector); */

/*** Request Handlers ***/

static void ata_read(uint32_t caller, struct request *req) {
	uint32_t sector, size, offset;
	uint8_t dr;

	if (!req) tail(caller, SIG_ERROR, NULL);

	dr = req->resource;
	offset = req->fileoff[0] % SECTSIZE;
	sector = req->fileoff[0] - offset;
	size = (req->datasize + offset > SECTSIZE) ? SECTSIZE - offset : req->datasize;

	if (!(drive[dr].flags & FLAG_EXIST)) {
		tail(caller, SIG_ERROR, NULL);
	}

	pio_read_sector(dr, sector, (uint16_t*) &req->reqdata[STDOFF - HDRSZ]);

	req->format   = REQ_WRITE;
	req->dataoff  = STDOFF + offset;
	req->datasize = size;
	tail(caller, SIG_REPLY, req);
}

static void ata_write(uint32_t caller, struct request *req) {
	uint32_t sector, size, offset;
	uint8_t *buffer;
	uint8_t dr;

	if (!req) tail(caller, SIG_ERROR, NULL);

	dr = req->resource;
	offset = req->fileoff[0] % SECTSIZE;
	sector = req->fileoff[0] - offset;
	size = (req->datasize + offset > SECTSIZE) ? SECTSIZE - offset : req->datasize;

	if (!(drive[dr].flags & FLAG_EXIST)) {
		tail(caller, SIG_ERROR, NULL);
	}

	buffer = malloc(SECTSIZE);

	pio_read_sector (dr, sector, (uint16_t*) buffer);
	memcpy(&buffer[offset], &req->reqdata[req->dataoff - HDRSZ], size);
	pio_write_sector(dr, sector, (uint16_t*) buffer);

	free(buffer);

	req->format   = REQ_READ;
	req->dataoff  = STDOFF;
	req->datasize = size;
	tail(caller, SIG_REPLY, req);
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
		select(dr);
		drive[dr].flags = 0;

		/* send IDENTIFY command */
		outb(ata_base[dr] + REG_CMD, CMD_ID);
		sleep400(dr);

		/* read status */
		status = inb(ata_base[dr] + REG_STAT);

		/* check for drive response */
		if (!status) {
			continue;
		}
		else {
			drive[dr].flags |= FLAG_EXIST;
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

		/* try to catch ATAPI devices */
		if (err) {
			cl = inb(ata_base[dr] + REG_LBA1);
			ch = inb(ata_base[dr] + REG_LBA2);
			c = cl | (ch << 8);

			if (c == 0xEB14 || c == 0x9669) {
				drive[dr].flags |= FLAG_ATAPI;
			}
			else {
				/* unknown device - exit */
				drive[dr].flags = 0;
				continue;
			}

			/* use packet-based IDENTIFY */
			outb(ata_base[dr] + REG_CMD, CMD_ID_ATAPI);
			sleep400(dr);
		}

		/* read in IDENTIFY space */
		for (i = 0; i < 256; i++) {
			buffer[i] = inw(ata_base[dr] + REG_DATA);
		}

		drive[dr].signature    = *((uint16_t*) &buffer[ID_TYPE]);
		drive[dr].capabilities = *((uint16_t*) &buffer[ID_CAP]);
		drive[dr].commandsets  = *((uint32_t*) &buffer[ID_CMDSET]);

		/* get LBA mode and disk size */
		if (drive[dr].commandsets & (1 << 26)) {
			drive[dr].flags |= FLAG_LONG;
			drive[dr].size[0] = *((uint32_t*) &buffer[ID_MAX_LBA48]);
			drive[dr].size[1] = *((uint16_t*) &buffer[ID_MAX_LBA48 + 4]);
		}
		else {
			drive[dr].size[0] = *((uint32_t*) &buffer[ID_MAX_LBA]);
			drive[dr].size[1] = 0;
		}

		/* get model string and null-terminate */
		for (i = 0; i < 40; i += 2) {
			drive[dr].model[i]   = buffer[ID_MODEL + (i / 2)] >> 8;
			drive[dr].model[i+1] = buffer[ID_MODEL + (i / 2)] & 0xFF;
		}
		drive[dr].model[40] = '\0';

		/* remove trailing spaces from model string */
		for (i = 39; i; i--) {
			if (drive[dr].model[i] == ' ') {
				drive[dr].model[i] = '\0';
			}
			else {
				break;
			}
		}

		/* detect DMA capability */
		/*status = inb(ata_dma[dr] + REG_DMA_STAT);
		if ((SEL(dr) == MASTER) && (status & DMA_STAT_MDMA)) {
			drive[dr].flags |= FLAG_DMA;
		}
		if ((SEL(dr) == SLAVE)  && (status & DMA_STAT_SDMA)) {
			drive[dr].flags |= FLAG_DMA;
		}*/

		printf("found drive: ");

		switch (dr) {
		case ATA00: printf("ATA 0 Master\n"); break;
		case ATA01: printf("ATA 0 Slave\n");  break;
		case ATA10: printf("ATA 1 Master\n"); break;
		case ATA11: printf("ATA 1 Slave\n");  break;
		}

		printf("\t");
		printf((drive[dr].flags & FLAG_ATAPI) ? "ATAPI " : "ATA ");
		printf((drive[dr].flags & FLAG_LONG) ? "LBA48 " : "LBA28 ");
		printf((drive[dr].flags & FLAG_DMA) ? "DMA " : "");
		printf("\n");

		printf("\tsize: %x KB\n", drive[dr].size[0] * SECTSIZE / 1024);
		printf("\tmodel: %s\n", drive[dr].model);
	}
	
	/* register interface handler */
	sigregister(SIG_READ, ata_read);
}

static void ata_halt(void) {
	return;
}
