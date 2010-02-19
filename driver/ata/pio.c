/* 
 * Copyright 2010 Nick Johnson 
 * ISC License, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/signal.h>

#include <stdint.h>

#include <driver/ata.h>

static void ata_pio_write  (uint8_t drive, uint64_t sector, uint16_t *buffer);
static void ata_pio_read   (uint8_t drive, uint64_t sector, uint16_t *buffer);
static void atapi_pio_write(uint8_t drive, uint64_t sector, uint16_t *buffer);
static void atapi_pio_read (uint8_t drive, uint64_t sector, uint16_t *buffer);

void pio_write_sector(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	ata_pio_write(drive, sector, buffer);
}

void pio_read_sector(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	ata_pio_read(drive, sector, buffer);
}

static void ata_pio_read(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	size_t i;
	uint8_t err;
	bool lba48 = false;

	sigblock(true, VSIG_REQ);

	/* send LBA to controller */
	lba48 = ata_send_lba(drive, sector);

	if (lba48) {
		outb(ata_base[drive] + REG_CMD, CMD_READ_PIO48);
	}
	else {
		outb(ata_base[drive] + REG_CMD, CMD_READ_PIO);
	}

	/* read in one sector of words */
	for (i = 0; i < (1 << ata_drive[drive].sectsize) >> 1; i++) {
		ata_sleep400(drive);
		while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
		buffer[i] = inw(ata_base[drive] + REG_DATA);
	}

	sigblock(false, VSIG_REQ);
}

static void atapi_pio_read(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	uint8_t atapi_cmd[12];

	atapi_cmd[0]  = CMD_READ_ATAPI;
	atapi_cmd[1]  = 0;
	atapi_cmd[2]  = (sector >> 24) & 0xFF;
	atapi_cmd[3]  = (sector >> 16) & 0xFF;
	atapi_cmd[4]  = (sector >> 8)  & 0xFF;
	atapi_cmd[5]  = (sector >> 0)  & 0xFF;
	atapi_cmd[6]  = 0;
	atapi_cmd[7]  = 0;
	atapi_cmd[8]  = 0;
	atapi_cmd[9]  = 1;
	atapi_cmd[10] = 0;
	atapi_cmd[11] = 0;

	sigblock(true, VSIG_REQ);

	ata_select(drive);

	sigblock(false, VSIG_REQ);
}

static void ata_pio_write(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	size_t i;
	uint8_t err;
	bool lba48 = false;

	/* must not be interrupted */
	sigblock(true, VSIG_REQ);

	lba48 = ata_send_lba(drive, sector);

	if (lba48) {
		outb(ata_base[drive] + REG_CMD, CMD_WRITE_PIO48);
	}
	else {
		outb(ata_base[drive] + REG_CMD, CMD_WRITE_PIO);
	}

	/* write one sector of words */
	for (i = 0; i < (1 << ata_drive[drive].sectsize) >> 1; i++) {
		ata_sleep400(drive);
		while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
		outw(ata_base[drive] + REG_DATA, buffer[i]);
	}

	/* flush write cache */
	outb(ata_base[drive] + REG_CMD, CMD_CACHE_FLUSH);
	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	sigblock(false, VSIG_REQ);
}
