/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>
#include <stdlib.h>
#include <signal.h>

#include <driver/ata.h>

static void ata_init(device_t selector);
static void ata_halt(void);
static void ata_hand(uint32_t caller, void *grant);

static device_t dev;
static uint16_t port;

struct driver_interface ata = {
	ata_init,
	ata_halt,

	NULL, 0,
};

static void ata_init(device_t selector) {
	dev 	= selector;

	rirq(dev_getirqnum(dev));
	sigregister(SSIG_IRQ, ata_hand);

	port = dev_getiobase(dev, 0);
}

static void ata_halt(void) {
	return;
}

static void ata_hand(uint32_t caller, void *grant) {
	return;
}
