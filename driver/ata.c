/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>
#include <stdlib.h>

#include <driver/ata.h>

#define ATA0_BASE 0x1F0
#define ATA1_BASE 0x170
#define ATA0_IRQ 14
#define ATA1_IRQ 15

static int ata_init(device_t selector);
static int ata_sleep(void);
static int ata_halt(void);

static int ata_read (struct request *r, callback_t cb);
static int ata_write(struct request *r, callback_t cb);
static int ata_ctrl (struct request *r, callback_t cb);
static int ata_info (struct request *r, callback_t cb);

static void ata_work(void);
static void ata_handler(void);

static device_t dev;
static uint16_t port_status;
static uint16_t port_base;

struct driver_interface ata = {
	ata_init,
	ata_sleep,
	ata_halt,

	ata_read,
	ata_write,
	ata_ctrl,
	ata_info,

	ata_work,
	0,

	ata_handler,
	-1,
};

static int ata_init(device_t selector) {	
	dev = selector;

	ata.irq = dev_getirqnum(dev);
	port_status = dev_getiobase(dev, 1);
	port_base = dev_getiobase(dev, 0);

	return DRV_DONE;
}

static int ata_sleep(void) {
	return DRV_ERROR;
}

static int ata_halt(void) {
	return DRV_ERROR;
}

static int ata_read(struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int ata_write(struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int ata_ctrl(struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static int ata_info(struct request *r, callback_t cb) {
	return DRV_ERROR;
}

static void ata_work(void) {
	return;
}

static void ata_handler(void) {
	return;
}
