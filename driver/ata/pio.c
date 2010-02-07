/* Copyright 2010 Nick Johnson */

#include <stdint.h>
#include <flux.h>

#include <driver/ata.h>

void pio_read_sector(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	size_t i;
	uint8_t err;
	uint8_t lba[3], head;

	/* must not be interrupted */
	sigblock(true);

	/* format LBA bytes from sector index */
	lba[0] = (sector >> 0)  & 0xFF;
	lba[1] = (sector >> 8)  & 0xFF;
	lba[2] = (sector >> 16) & 0xFF;
	head   = (sector >> 24) & 0xF;

	/* wait for drive to be ready */
	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	/* select drive, send LBA, and send read command */
	outb(ata_base[drive] + REG_SELECT, (SEL(drive) ? 0xF0 : 0xE0) | head);
	outb(ata_base[drive] + REG_COUNT0, 0x01);
	outb(ata_base[drive] + REG_LBA0, lba[0]);
	outb(ata_base[drive] + REG_LBA1, lba[1]);
	outb(ata_base[drive] + REG_LBA2, lba[2]);
	outb(ata_base[drive] + REG_CMD, CMD_READ_PIO);

	/* read in one sector of words */
	for (i = 0; i < SECTSIZE / sizeof(uint16_t); i++) {
		ata_sleep400(drive);
		while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
		buffer[i] = inw(ata_base[drive] + REG_DATA);
	}

	sigblock(false);
}

void pio_write_sector(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	size_t i;
	uint8_t err;
	uint8_t lba[3], head;

	/* must not be interrupted */
	sigblock(true);

	/* format LBA bytes from sector index */
	lba[0] = (sector >> 0)  & 0xFF;
	lba[1] = (sector >> 8)  & 0xFF;
	lba[2] = (sector >> 16) & 0xFF;
	head   = (sector >> 24) & 0xF;

	/* wait for drive to be ready */
	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	/* select drive, send LBA, and send write command */
	outb(ata_base[drive] + REG_SELECT, (SEL(drive) ? 0xF0 : 0xE0) | head);
	outb(ata_base[drive] + REG_COUNT0, 0x01);
	outb(ata_base[drive] + REG_LBA0, lba[0]);
	outb(ata_base[drive] + REG_LBA1, lba[1]);
	outb(ata_base[drive] + REG_LBA2, lba[2]);
	outb(ata_base[drive] + REG_CMD, CMD_WRITE_PIO);

	/* write one sector of words */
	for (i = 0; i < SECTSIZE / sizeof(uint16_t); i++) {
		ata_sleep400(drive);
		while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);
		outw(ata_base[drive] + REG_DATA, buffer[i]);
	}

	/* flush write cache */
	outb(ata_base[drive] + REG_CMD, CMD_CACHE_FLUSH);
	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	sigblock(false);
}
