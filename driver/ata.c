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
#define ID_CYLINDERS		0x02
#define ID_HEADS			0x06
#define ID_SECTORS			0x0C
#define ID_SERIAL			0x14
#define ID_MODEL			0x36
#define ID_CAPABILITIES		0x62
#define ID_FIELDVALID		0x6A
#define ID_MAX_LBA			0x78
#define ID_COMMANDSETS		0xA4
#define ID_MAX_LBA_EXT		0xC8

#define FLAG_EXIST			0x01
#define FLAG_DMA			0x02
#define FLAG_ATAPI			0x04
#define FLAG_LBA			0x08
#define FLAG_LONG			0x10

static void ata_init(device_t selector);
static void ata_halt(void);

/*** Device Information ***/

static device_t ata_controller;

static struct drive {
	uint8_t  flags; /* see FLAG_* */
	uint8_t  index; /* bit 0 - master/slave; bit 1 - primary/secondary */
	uint16_t signature;
	uint16_t capabilities;
	uint16_t commandsets;
	uint32_t size[2];
	char model[41];
} drives[4];

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

	/* disable controller IRQs */
	ata_out(ATA0_MASTER, REG_CONTROL, CTRL_NEIN);
	ata_out(ATA1_MASTER, REG_CONTROL, CTRL_NEIN);

	sigregister(SIG_READ, ata_read);
}

static void ata_halt(void) {
	return;
}
