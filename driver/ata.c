/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>
#include <stdlib.h>
#include <signal.h>

#include <driver/ata.h>

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

struct driver_interface ata = {
	ata_init,
	ata_halt,

	NULL, 0,
};

static void ata_init(device_t selector) {

	/* currently only supports ATA1 master */
	rirq(ATA1_IRQ);
	port = ATA1_BASE;

	inb(port + RSTAT);
}

static void ata_halt(void) {
	return;
}
