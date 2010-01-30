/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include <driver/ata.h>
#include <driver/pci.h>

#define ATA0_IRQ 14
#define ATA1_IRQ 15
#define ATA2_IRQ -1
#define ATA3_IRQ -1

#define ATA0_BASE 0x1F0
#define ATA1_BASE 0x170
#define ATA2_BASE 0x1E8
#define ATA3_BASE 0x368

#define MASTR 0
#define SLAVE 1

#define DATA  0
#define ERROR 1
#define COUNT 2
#define ADDR0 3
#define ADDR1 4
#define ADDR2 5
#define DRSEL 6
#define RSTAT 7
#define ASTAT 0x206

#define RSTAT_BUSY  0x80
#define RSTAT_READY 0x40
#define RSTAT_FAULT 0x20
#define RSTAT_SREQ  0x10
#define RSTAT_DREQ  0x08
#define RSTAT_ERROR 0x04
#define ASTAT_HIGHB 0x80
#define ASTAT_RESET 0x04
#define ASTAT_CLIRQ 0x02

static void ata_init(device_t selector);
static void ata_halt(void);

static uint32_t dev;
static uint16_t port;

/*** Helper Routines ***/

static void ata_reset(void) {
	uint8_t astat;

	astat = inb(port + ASTAT);
	outb(port + ASTAT, astat | ASTAT_RESET);
	outb(port + ASTAT, astat);
}

static void ata_select(int drive) {
	
}

/*** Request Handlers ***/

static void ata_read(uint32_t caller, struct request *req) {
	printf("ATA: attempted read at %x size %x\n", req->fileoff[0], req->datasize);

	req->format = REQ_WRITE;
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

	sigregister(SIG_READ, ata_read);
}

static void ata_halt(void) {
	return;
}
